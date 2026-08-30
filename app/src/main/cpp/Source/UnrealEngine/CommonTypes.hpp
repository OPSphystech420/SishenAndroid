#pragma once

#include "Containers.hpp"

struct FVector
{
public:
    float X;
    float Y;
    float Z;

public:
    FVector() : X(0), Y(0), Z(0) {}
    FVector(float v) : X(v), Y(v), Z(v) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}

    FVector Normalize()
    {
        *this /= Magnitude();
        return *this;
    }

    FVector operator*=(const FVector& Other)
    {
        *this = *this * Other;
        return *this;
    }

    FVector operator*=(float Scalar)
    {
        *this = *this * Scalar;
        return *this;
    }

    FVector operator+=(const FVector& Other)
    {
        *this = *this + Other;
        return *this;
    }

    FVector operator-=(const FVector& Other)
    {
        *this = *this - Other;
        return *this;
    }

    FVector operator/=(const FVector& Other)
    {
        *this = *this / Other;
        return *this;
    }

    FVector operator/=(float Scalar)
    {
        *this = *this / Scalar;
        return *this;
    }

    float Dot(const FVector& Other) const
    {
        return (X * Other.X) + (Y * Other.Y) + (Z * Other.Z);
    }

    float GetDistanceTo(const FVector& Other) const
    {
        FVector DiffVector = Other - *this;
        return DiffVector.Magnitude();
    }

    float GetDistanceToInMeters(const FVector& Other) const
    {
        return GetDistanceTo(Other) * 0.01;
    }

    bool IsNearlyZero(float Tolerance = 1.e-4f) const
    {
        return SizeSquared() <= Tolerance * Tolerance;
    }

    FVector GetNormalized() const
    {
        return *this / Magnitude();
    }

    float SizeSquared() const
    {
        return X * X + Y * Y + Z * Z;
    }

    FVector GetSafeNormal(float Tolerance = 1.e-8f) const
    {
        const float SquareSum = X*X + Y*Y + Z*Z;
        if (SquareSum == 1.f)
            return *this;	
        else if (SquareSum < Tolerance)
            return FVector();
        
        const float Scale = 1.0f / std::sqrt(SquareSum);
        return FVector(X*Scale, Y*Scale, Z*Scale);
    }

    bool IsZero() const
    {
        return X == 0.0 && Y == 0.0 && Z == 0.0;
    }

    float Magnitude() const
    {
        return std::sqrt((X * X) + (Y * Y) + (Z * Z));
    }

    bool operator!=(const FVector& Other) const
    {
        return X != Other.X || Y != Other.Y || Z != Other.Z;
    }

    FVector operator*(const FVector& Other) const
    {
        return {X * Other.X, Y * Other.Y, Z * Other.Z};
    }

    FVector operator*(float Scalar) const
    {
        return {X * Scalar, Y * Scalar, Z * Scalar};
    }

    FVector operator+(const FVector& Other) const
    {
        return {X + Other.X, Y + Other.Y, Z + Other.Z};
    }

    FVector operator-(const FVector& Other) const
    {
        return {X - Other.X, Y - Other.Y, Z - Other.Z};
    }

    FVector operator/(const FVector& Other) const
    {
        return {X / Other.X, Y / Other.Y, Z / Other.Z};
    }

    FVector operator/(float Scalar) const
    {
        return {X / Scalar, Y / Scalar, Z / Scalar};
    }

    bool operator==(const FVector& Other) const
    {
        return X == Other.X && Y == Other.Y && Z == Other.Z;
    }

    FVector operator^(const FVector& V) const
    {
        return FVector(Y * V.Z - Z * V.Y, Z * V.X - X * V.Z, X * V.Y - Y * V.X);
    }

    float operator|(const FVector& V) const
    {
        return X * V.X + Y * V.Y + Z * V.Z;
    }

    static FVector CrossProduct(const FVector& A, const FVector& B)
    {
        return A ^ B;
    }

    struct FRotator ToOrientationRotator() const;

    struct FRotator Rotation() const;


    static FVector Zero;
};

struct FVector2D final
{
public: 
	float X; 
	float Y;

public:
	FVector2D() : X(0), Y(0) {}
	FVector2D(float v) : X(v), Y(v) {}
	FVector2D(float x, float y) : X(x), Y(y) {}

    FORCEINLINE operator ImVec2() const
    {
        return ImVec2(X, Y);
    }

	FVector2D& operator*=(const FVector2D& Other)
	{
        *this = *this * Other;
        return *this;
    }

	FVector2D& operator*=(float Scalar)
	{
        *this = *this * Scalar;
        return *this;
    }

	FVector2D& operator+=(const FVector2D& Other)
	{
        *this = *this + Other;
        return *this;
    }

	FVector2D& operator-=(const FVector2D& Other)
	{
        *this = *this - Other;
        return *this;
    }

	FVector2D& operator/=(const FVector2D& Other)
	{
        *this = *this / Other;
        return *this;
    }

	FVector2D& operator/=(float Scalar)
	{
        *this = *this / Scalar;
        return *this;
    }

    float GetDistanceTo(const FVector2D& Other) const
    {
        FVector2D DiffVector = Other - *this;
        return DiffVector.Magnitude();
    }

    static float GetDistance(const FVector2D& A, const FVector2D& B)
    {
        return A.GetDistanceTo(B);
    }

    float GetDistanceToInMeters(const FVector2D& Other) const
    {
        return GetDistanceTo(Other) * 0.01;
    }

    FVector2D GetNormalized() const
    {
        return *this / Magnitude();
    }

    bool IsZero() const
    {
        return X == 0.0 && Y == 0.0;
    }

    float Magnitude() const
    {
        return std::sqrt((X * X) + (Y * Y));
    }

	bool operator!=(const FVector2D& Other) const
	{
        return X != Other.X || Y != Other.Y;
    }

	FVector2D operator*(const FVector2D& Other) const
	{
        return {X * Other.X, Y * Other.Y};
    }

	FVector2D operator*(float Scalar) const
	{
        return {X * Scalar, Y * Scalar};
    }

	FVector2D operator+(const FVector2D& Other) const
	{
        return {X + Other.X, Y + Other.Y};
    }

	FVector2D operator-(const FVector2D& Other) const
	{
        return {X - Other.X, Y - Other.Y};
    }

	FVector2D operator/(const FVector2D& Other) const
	{
        return {X / Other.X, Y / Other.Y};
    }

	FVector2D operator/(float Scalar) const
	{
        return {X / Scalar, Y / Scalar};
    }

	bool operator==(const FVector2D& Other) const
	{
        return X == Other.X && Y == Other.Y;
    }

    static FVector2D Zero;
};

struct FQuat final
{
    float X, Y, Z, W; 

    FVector operator*(const FVector& V) const
    {
        return RotateVector(V);
    }

    FVector RotateVector(const FVector& V) const
    {
        // http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
        // V' = V + 2w(Q x V) + (2Q x (Q x V))
        // refactor:
        // V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
        // T = 2(Q x V);
        // V' = V + w*(T) + (Q x T)

        const FVector Q(X, Y, Z);
        const FVector T = FVector::CrossProduct(Q, V) * 2.0f;
        return V + (T * W) + FVector::CrossProduct(Q, T);
    }
    
    struct FRotator Rotator() const;
    
    FORCEINLINE FVector GetAxisX() const
    {
        return RotateVector(FVector(1.f, 0.f, 0.f));
    }


    FORCEINLINE FVector GetAxisY() const
    {
        return RotateVector(FVector(0.f, 1.f, 0.f));
    }


    FORCEINLINE FVector GetAxisZ() const
    {
        return RotateVector(FVector(0.f, 0.f, 1.f));
    }
};

struct FPlane 
{
    float X, Y, Z, W;

    FPlane() = default;
    FPlane(float _X, float _Y, float _Z, float _W) : X(_X), Y(_Y), Z(_Z), W(_W) {}
};

namespace EAxis
{
    enum Type
    {
        X,
        Y,
        Z
    };
};

struct FMatrix
{
    union
    {
        struct
        {
            FPlane XPlane;
            FPlane YPlane;
            FPlane ZPlane;
            FPlane WPlane;
        };
        float M[4][4];
    };



    FMatrix() = default;
    FMatrix(FPlane X, FPlane Y, FPlane Z, FPlane W) : XPlane(X), YPlane(Y), ZPlane(Z), WPlane(W) {}

    FMatrix(const FVector& InX,const FVector& InY,const FVector& InZ,const FVector& InW)
    {
        M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = 0.0f;
        M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = 0.0f;
        M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = 0.0f;
        M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = 1.0f;
    }
    
    float *operator[](int Index) 
    {
        return M[Index];
    }

    const float* operator[](int Index) const 
    {
        return M[Index];
    }

    FMatrix operator*(const FMatrix& other)
    {
        FMatrix Ret;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < 4; k++)
                {
                    Ret.M[i][j] += M[i][k] * other.M[k][j];
                }
            }
        }
        return Ret;
    }

    FVector GetOrigin() const
    {
        return FVector(M[3][0], M[3][1], M[3][2]);
    }

    FORCEINLINE FVector GetScaledAxis( EAxis::Type InAxis ) const
    {
        switch ( InAxis )
        {
            case EAxis::X:
                return FVector(M[0][0], M[0][1], M[0][2]);

            case EAxis::Y:
                return FVector(M[1][0], M[1][1], M[1][2]);

            case EAxis::Z:
                return FVector(M[2][0], M[2][1], M[2][2]);

            default:
                return FVector::Zero;
        }
    }

    struct FRotator Rotator() const;

    static FMatrix MakeFromX(FVector const& XAxis)
    {
        constexpr float KINDA_SMALL_NUMBER = 0.00001;

        FVector const NewX = XAxis.GetSafeNormal();

        // try to use up if possible
        FVector const UpVector = ( std::abs(NewX.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? FVector(0,0,1.f) : FVector(1.f,0,0);

        const FVector NewY = (UpVector ^ NewX).GetSafeNormal();
        const FVector NewZ = NewX ^ NewY;

        return FMatrix(NewX, NewY, NewZ, FVector::Zero);
    }

    static FMatrix Identity;
};



struct FRotator final
{
    float Pitch, Yaw, Roll;

    FRotator() = default;
    FRotator(float _Pitch, float _Yaw, float _Roll = 0.0f) : Pitch(_Pitch), Yaw(_Yaw), Roll(_Roll) {}

    FVector Vector() const
    {
        float PitchNoWinding = std::fmodf(Pitch, 360.0f);
        float YawNoWinding = std::fmodf(Yaw, 360.0f);

        const float RadPitch = PitchNoWinding * (M_PI / 180.0f);
        const float RadYaw = YawNoWinding * (M_PI / 180.0f);

        float SP = std::sin(RadPitch), SY = std::sin(RadYaw);
        float CP = std::cos(RadPitch), CY = std::cos(RadYaw);

        return FVector(CP * CY, CP * SY, SP);
    }

    FMatrix ToMatrix() const
    {
        float PitchRad = Pitch * DEGTORAD;
        float YawRad   = Yaw * DEGTORAD;
        float RollRad  = Roll * DEGTORAD;

        float SP = std::sin(PitchRad);
        float CP = std::cos(PitchRad);
        float SY = std::sin(YawRad);
        float CY = std::cos(YawRad);
        float SR = std::sin(RollRad);
        float CR = std::cos(RollRad);

        FMatrix RotationMatrix;

        RotationMatrix.M[0][0] = CP * CY;
        RotationMatrix.M[0][1] = CP * SY;
        RotationMatrix.M[0][2] = SP;
        RotationMatrix.M[0][3] = 0.f;

        RotationMatrix.M[1][0] = SR * SP * CY - CR * SY;
        RotationMatrix.M[1][1] = SR * SP * SY + CR * CY;
        RotationMatrix.M[1][2] = -SR * CP;
        RotationMatrix.M[1][3] = 0.f;

        RotationMatrix.M[2][0] = -(CR * SP * CY + SR * SY);
        RotationMatrix.M[2][1] = CY * SR - CR * SP * SY;
        RotationMatrix.M[2][2] = CR * CP;
        RotationMatrix.M[2][3] = 0.f;

        RotationMatrix.M[3][0] = 0.f;
        RotationMatrix.M[3][1] = 0.f;
        RotationMatrix.M[3][2] = 0.f;
        RotationMatrix.M[3][3] = 1.f;
        return RotationMatrix;
    }

    FQuat ToQuaterion() const
    {
        const float HalfDegToRad = DEGTORAD / 2.f;

        const float PitchNoWinding = std::fmodf(Pitch, 360.0f) * HalfDegToRad;
        const float YawNoWinding = std::fmodf(Yaw, 360.0f) * HalfDegToRad;
        const float RollNoWinding = std::fmodf(Roll, 360.0f) * HalfDegToRad;

        float SP = std::sinf(PitchNoWinding), SY = std::sinf(YawNoWinding), SR = std::sinf(RollNoWinding);
        float CP = std::cosf(PitchNoWinding), CY = std::cosf(YawNoWinding), CR = std::cosf(RollNoWinding);

        FQuat RotationQuat;
        RotationQuat.X =  CR*SP*SY - SR*CP*CY;
        RotationQuat.Y = -CR*SP*CY - SR*CP*SY;
        RotationQuat.Z =  CR*CP*SY - SR*SP*CY;
        RotationQuat.W =  CR*CP*CY + SR*SP*SY;
        return RotationQuat;
    }
    
    static float ClampAxis( float Angle )
    {
        // returns Angle in the range (-360,360)
        Angle = std::fmod(Angle, 360.f);

        if (Angle < 0.f)
        {
            // shift to [0,360) range
            Angle += 360.f;
        }

        return Angle;
    }

    static float NormalizeAxis( float Angle )
    {
        // returns Angle in the range [0,360)
        Angle = ClampAxis(Angle);

        if (Angle > 180.f)
        {
            // shift to (-180,180]
            Angle -= 360.f;
        }

        return Angle;
    }
};


struct FTransform final
{
    // Dependant on game and its padding alignment
    // Add the paddings that are dumped
    FQuat Rotation;
    FVector Translation;
    FVector Scale3D;
    uint8 Pad[0x8];
    
    
    inline FMatrix ToMatrixWithScale() const
	{
		FMatrix OutMatrix;

		OutMatrix.M[3][0] = Translation.X;
		OutMatrix.M[3][1] = Translation.Y;
		OutMatrix.M[3][2] = Translation.Z;

		const float x2 = Rotation.X + Rotation.X;
		const float y2 = Rotation.Y + Rotation.Y;
		const float z2 = Rotation.Z + Rotation.Z;
		{
			const float xx2 = Rotation.X * x2;
			const float yy2 = Rotation.Y * y2;
			const float zz2 = Rotation.Z * z2;

			OutMatrix.M[0][0] = (1.0f - (yy2 + zz2)) * Scale3D.X;
			OutMatrix.M[1][1] = (1.0f - (xx2 + zz2)) * Scale3D.Y;
			OutMatrix.M[2][2] = (1.0f - (xx2 + yy2)) * Scale3D.Z;
		}
		{
			const float yz2 = Rotation.Y * z2;
			const float wx2 = Rotation.W * x2;

			OutMatrix.M[2][1] = (yz2 - wx2) * Scale3D.Z;
			OutMatrix.M[1][2] = (yz2 + wx2) * Scale3D.Y;
		}
		{
			const float xy2 = Rotation.X * y2;
			const float wz2 = Rotation.W * z2;

			OutMatrix.M[1][0] = (xy2 - wz2) * Scale3D.Y;
			OutMatrix.M[0][1] = (xy2 + wz2) * Scale3D.X;
		}
		{
			const float xz2 = Rotation.X * z2;
			const float wy2 = Rotation.W * y2;

			OutMatrix.M[2][0] = (xz2 + wy2) * Scale3D.Z;
			OutMatrix.M[0][2] = (xz2 - wy2) * Scale3D.X;
		}

		OutMatrix.M[0][3] = 0.0f;
		OutMatrix.M[1][3] = 0.0f;
		OutMatrix.M[2][3] = 0.0f;
		OutMatrix.M[3][3] = 1.0f;

		return OutMatrix;
	}

    inline FMatrix ToMatrixNoScale() const
	{
		FMatrix OutMatrix;

		OutMatrix.M[3][0] = Translation.X;
		OutMatrix.M[3][1] = Translation.Y;
		OutMatrix.M[3][2] = Translation.Z;

		const float x2 = Rotation.X + Rotation.X;
		const float y2 = Rotation.Y + Rotation.Y;
		const float z2 = Rotation.Z + Rotation.Z;
		{
			const float xx2 = Rotation.X * x2;
			const float yy2 = Rotation.Y * y2;
			const float zz2 = Rotation.Z * z2;

			OutMatrix.M[0][0] = (1.0f - (yy2 + zz2));
			OutMatrix.M[1][1] = (1.0f - (xx2 + zz2));
			OutMatrix.M[2][2] = (1.0f - (xx2 + yy2));
		}
		{
			const float yz2 = Rotation.Y * z2;
			const float wx2 = Rotation.W * x2;

			OutMatrix.M[2][1] = (yz2 - wx2);
			OutMatrix.M[1][2] = (yz2 + wx2);
		}
		{
			const float xy2 = Rotation.X * y2;
			const float wz2 = Rotation.W * z2;

			OutMatrix.M[1][0] = (xy2 - wz2);
			OutMatrix.M[0][1] = (xy2 + wz2);
		}
		{
			const float xz2 = Rotation.X * z2;
			const float wy2 = Rotation.W * y2;

			OutMatrix.M[2][0] = (xz2 + wy2);
			OutMatrix.M[0][2] = (xz2 - wy2);
		}

		OutMatrix.M[0][3] = 0.0f;
		OutMatrix.M[1][3] = 0.0f;
		OutMatrix.M[2][3] = 0.0f;
		OutMatrix.M[3][3] = 1.0f;

		return OutMatrix;
	}

    FVector operator*(const FTransform& Other) const
    {
        return Other.Rotation * (Other.Scale3D * Translation) + Other.Translation;
    }

    FVector GetLocation() const
    {
        return Translation;
    }
};

class FReferenceControllerBase
{
public:
    FORCEINLINE explicit FReferenceControllerBase()
        : SharedReferenceCount(1)
        , WeakReferenceCount(1)
    {
    }
    
    void** VTable;
    int32 SharedReferenceCount;
    int32 WeakReferenceCount;

    void DestoryObject() const
    {
        reinterpret_cast<void(*)(const FReferenceControllerBase*)>(VTable[1])(this);
    }

private:
    FReferenceControllerBase( FReferenceControllerBase const& );
    FReferenceControllerBase& operator=( FReferenceControllerBase const& );
};

template<typename T>
class TSharedPtr
{
public:
    T* Object;
    FReferenceControllerBase* ReferenceController;
};

template<typename ReferencedType>
class TRefCountPtr
{
    typedef ReferencedType* ReferenceType;
public:
    ReferencedType* Reference;
};

namespace ETextFlag
{
    enum Type
    {
        Transient = (1 << 0),
        CultureInvariant = (1 << 1),
        ConvertedProperty = (1 << 2),
        Immutable = (1 << 3),
        InitializedFromString = (1<<4),  // this ftext was initialized using FromString
    };
}

class FText final
{
public:
    static inline FText* StaticEmptyText = nullptr;
    
    // TSharedRef<ITextData, ESPMode::ThreadSafe> TextData;
    void* TextData;
    FReferenceControllerBase* ReferenceController;
    uint32 Flags;
    uint8 Pad[4];

public:
    
    FText() = default;
    FText(FString&& String);
    
public:
    
    FText& operator=(const FText& Other)
    {
        this->TextData = Other.TextData;
        this->ReferenceController = Other.ReferenceController;
        
        if (this->ReferenceController)
            InterlockedIncrement(&ReferenceController->SharedReferenceCount);
        
        this->Flags = Other.Flags;
        
        return *this;
    }
    
    
    const FString& GetDisplayString() const
    {
        void** VTable = *reinterpret_cast<void***>(TextData);
        auto GetDisplayString_Internal = (const FString&(*)(void*))(VTable[3]);
        return GetDisplayString_Internal(TextData);
    }

    std::string ToString() const
    {
        if ( TextData )
        {
            return GetDisplayString().ToString();
        }
        return "";
    }

    std::u16string ToWString() const
    {
        if ( TextData )
        {
            return GetDisplayString().ToWString();
        }
        return u"";
    }
    
    static FText GetEmpty()
    {
        static FText EmptyTextValue = FText(u"");
        return EmptyTextValue;
    }
    
    static FText FromString(const FString& InString);
    
    static FORCEINLINE int32 InterlockedDecrement(volatile int32* Value)
    {
        return __sync_fetch_and_sub(Value, 1) - 1;
    }
    
    static FORCEINLINE int32 InterlockedIncrement(volatile int32* Value)
    {
        return __sync_fetch_and_add(Value, 1) + 1;
    }

    
public:
    
    ~FText()
    {
        if (InterlockedDecrement(&ReferenceController->SharedReferenceCount) == 0)
        {
            ReferenceController->DestoryObject();
            
            if (InterlockedDecrement(&ReferenceController->WeakReferenceCount) == 0)
            {
                reinterpret_cast<void(*)(void*)>(ReferenceController->VTable[2])(ReferenceController);
            }
        }
    }
    
};

struct FColor
{
    uint8 R, G, B, A;

    FColor() = default;
    FColor(uint8 r, uint8 g, uint8 b, uint8 a = 255) : R(r), G(g), B(b), A(a) {}
    
    static const FColor White;
    static const FColor Black;
    static const FColor Transparent;
    static const FColor Red;
    static const FColor Green;
    static const FColor Blue;
    static const FColor Yellow;
    static const FColor Cyan;
    static const FColor Magenta;
    static const FColor Orange;
    static const FColor Purple;
    static const FColor Turquoise;
    static const FColor Silver;
    static const FColor Emerald;
};

struct FLinearColor 
{
    float R, G, B, A;
    
    /** Static lookup table used for FColor -> FLinearColor conversion. Pow(2.2) */
    static float Pow22OneOver255Table[256];

    /** Static lookup table used for FColor -> FLinearColor conversion. sRGB */
    static float sRGBToLinearTable[256];

    FLinearColor() = default;
    FLinearColor(float r, float g, float b, float a = 1.0f) : R(r), G(g), B(b), A(a) {}
    
    FLinearColor(const FColor& Color)
    {
        constexpr float OneOver255 = 1.0f / 255.0f;
        
        R = sRGBToLinearTable[Color.R];
        G = sRGBToLinearTable[Color.G];
        B = sRGBToLinearTable[Color.B];
        A = float(Color.A) * OneOver255;
    }

    
    static const FLinearColor White;
    static const FLinearColor Gray;
    static const FLinearColor Black;
    static const FLinearColor Transparent;
    static const FLinearColor Red;
    static const FLinearColor Green;
    static const FLinearColor Blue;
    static const FLinearColor Yellow;
    static const FLinearColor Cyan;
    static const FLinearColor Purple;
};


FORCEINLINE FRotator FVector::ToOrientationRotator() const
{
    FRotator Ret;
    Ret.Yaw    = std::atan2(Y, X) * (180.f / M_PI);
    Ret.Pitch  = std::atan2(Z, std::sqrt(X*X+Y+Y)) * (180.f / M_PI);
    Ret.Roll   = 0;

    return Ret;
}

FORCEINLINE FRotator FVector::Rotation() const
{
    return ToOrientationRotator();
}

FORCEINLINE FRotator FQuat::Rotator() const 
{
    const float SingularityTest = Z*X-W*Y;
    const float YawY = 2.f*(W*Z+X*Y);
    const float YawX = (1.f-2.f*((Y * 2) + (Z * 2)));

    // reference
    // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

    // this value was found from experience, the above websites recommend different values
    // but that isn't the case for us, so I went through different testing, and finally found the case
    // where both of world lives happily.
    const float SINGULARITY_THRESHOLD = 0.4999995f;
    const float RAD_TO_DEG = (180.f)/M_PI;
    FRotator RotatorFromQuat;

    if (SingularityTest < -SINGULARITY_THRESHOLD)
    {
        RotatorFromQuat.Pitch = -90.f;
        RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = FRotator::NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * std::atan2(X, W) * RAD_TO_DEG));
    }
    else if (SingularityTest > SINGULARITY_THRESHOLD)
    {
        RotatorFromQuat.Pitch = 90.f;
        RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = FRotator::NormalizeAxis(RotatorFromQuat.Yaw - (2.f * std::atan2(X, W) * RAD_TO_DEG));
    }
    else
    {
        RotatorFromQuat.Pitch = std::asin(2.f*(SingularityTest)) * RAD_TO_DEG;
        RotatorFromQuat.Yaw = std::atan2(YawY, YawX) * RAD_TO_DEG;
        RotatorFromQuat.Roll = std::atan2(-2.f*(W*X+Y*Z), (1.f-2.f*((X * 2) + (Y * 2)))) * RAD_TO_DEG;
    }

    return RotatorFromQuat;
}

FORCEINLINE FRotator FMatrix::Rotator() const
{
    const FVector XAxis	= GetScaledAxis( EAxis::X );
    const FVector YAxis	= GetScaledAxis( EAxis::Y );
    const FVector ZAxis	= GetScaledAxis( EAxis::Z );

    FRotator Rotator = FRotator(
            std::atan2( XAxis.Z, std::sqrt((XAxis.X * 2.0f) + (XAxis.Y * 2.0f)) ) * 180.f / M_PI,
            std::atan2( XAxis.Y, XAxis.X ) * 180.f / M_PI,
            0
    );

    return Rotator;
}