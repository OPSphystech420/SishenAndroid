package ark.use.sishen;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Build;
import android.provider.Settings;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.TimeZone;

public class GiveKey {
    private static final String TAG = "GiveKey";

    static {
        System.loadLibrary("Sishen");
    }

    public native void sendKeyToCpp(String key);
    public native void sendKeysToCpp(String[] keys);
    public native boolean validateKeyFromCpp();
    public native void setJavaContext(Context context, GiveKey giveKey);
    public native String getObfuscatedUrl();

    private static final String KEY_PREFIX = "ARKLIANTOOL";
    private static final int EXPIRATION_DATE_LENGTH = 10; // Format: dd_MM_yyyy (10 characters)
    private final Context context;
    private List<String> keysFromWebsite = new ArrayList<>();
    private String generatedKey = "";

    public GiveKey(Context context) {
        this.context = context;
    }

    public String generateKey() {
        String androidId = Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
        String deviceModel = Build.MODEL;
        String currentDate = getCurrentDate();
        generatedKey = KEY_PREFIX + androidId + deviceModel + "_" + currentDate;
        return generatedKey;
    }

    public static String getCurrentDate() {
        Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("UTC"), Locale.US);
        SimpleDateFormat sdf = new SimpleDateFormat("dd_MM_yyyy", Locale.US);
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        return sdf.format(cal.getTime());
    }

    public void genAndSendKeyToCpp() {
        String key = generateKey();
        sendKeyToCpp(key);
        setJavaContext(context, this);
    }

    public void fetchUrlFromCpp() {
        new FetchKeysTask().execute();
    }

    private class FetchKeysTask extends AsyncTask<Void, Void, String> {
        @Override
        protected String doInBackground(Void... voids) {
            HttpURLConnection connection = null;
            BufferedReader reader = null;

            try {
                String urlString = getObfuscatedUrl();
                URL url = new URL(urlString);
                connection = (HttpURLConnection) url.openConnection();
                connection.setConnectTimeout(5000);
                connection.setReadTimeout(5000);
                connection.setRequestMethod("GET");
                connection.connect();
                int responseCode = connection.getResponseCode();


                if (responseCode != HttpURLConnection.HTTP_OK) {
                    if (connection.getErrorStream() != null) {
                        BufferedReader errorReader = new BufferedReader(new InputStreamReader(connection.getErrorStream()));
                        String errorLine;
                        StringBuilder errorStringBuilder = new StringBuilder();
                        while ((errorLine = errorReader.readLine()) != null) {
                            errorStringBuilder.append(errorLine);
                        }
                    }
                    return null;
                }

                InputStream inputStream = connection.getInputStream();
                reader = new BufferedReader(new InputStreamReader(inputStream));
                StringBuilder stringBuilder = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    stringBuilder.append(line).append('\n');
                }
                return stringBuilder.toString();

            } catch (Exception e) {
                return null;
            } finally {
                try {
                    if (reader != null) reader.close();
                } catch (Exception e) {
                }
                if (connection != null) connection.disconnect();
            }
        }

        @Override
        protected void onPostExecute(String websiteContent) {
            if (websiteContent != null) {
                String[] keyArray = websiteContent.split(";");
                keysFromWebsite.clear();
                for (String key : keyArray) {
                    key = key.trim();
                    if (!key.isEmpty()) {
                        keysFromWebsite.add(key);
                    }
                }
                sendKeysToCpp(keysFromWebsite.toArray(new String[0]));
            }
            genAndSendKeyToCpp();
        }
    }

    // Updated validation: iterate through keysFromWebsite and compare the generated key without expiration.
    public boolean validateKey() {
        if (generatedKey.length() < EXPIRATION_DATE_LENGTH) {
            return false;
        }
        String generatedKeyWithoutDate = generatedKey.substring(0, generatedKey.length() - EXPIRATION_DATE_LENGTH);
        boolean valid = false;
        for (String key : keysFromWebsite) {
            if (key.length() >= EXPIRATION_DATE_LENGTH) {
                String websiteKeyWithoutDate = key.substring(0, key.length() - EXPIRATION_DATE_LENGTH);
                if (generatedKeyWithoutDate.equals(websiteKeyWithoutDate)) {
                    String expDateStr = key.substring(key.length() - EXPIRATION_DATE_LENGTH);
                    if (isKeyValidForDate(expDateStr)) {
                        valid = true;
                        break;
                    }
                }
            }
        }
        return valid;
    }

    private boolean isKeyValidForDate(String expDateStr) {
        if (expDateStr == null || expDateStr.length() != EXPIRATION_DATE_LENGTH) {
            return false;
        }
        SimpleDateFormat sdf = new SimpleDateFormat("dd_MM_yyyy", Locale.US);
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        try {
            Date expiryDate = sdf.parse(expDateStr);
            Calendar nowCal = Calendar.getInstance(TimeZone.getTimeZone("UTC"), Locale.US);
            Calendar expiryCal = Calendar.getInstance(TimeZone.getTimeZone("UTC"), Locale.US);
            expiryCal.setTime(expiryDate);
            if (nowCal.before(expiryCal)) {
                return true;
            } else {

                return false;
            }
        } catch (ParseException e) {
            return false;
        }
    }
}