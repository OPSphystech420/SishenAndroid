package ark.use.sishen;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;

public class ClipBoard {
    private static ClipboardManager clipboardManager;

    public static void initialize(Context context) {
        clipboardManager = (ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
    }

    public static void copyToClipboard(String text) {
        if (clipboardManager != null) {
            ClipData clip = ClipData.newPlainText("Copied Text", text);
            clipboardManager.setPrimaryClip(clip);
        }
    }
}