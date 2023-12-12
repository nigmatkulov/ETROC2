#ifndef Hit_t
#define Hit_t

// ROOT headers
#include <TObject.h>
#include <Rtypes.h>

//________________
class Hit : public TObject {
    
  public:
    /// @brief Default constructor
    Hit();
    /// @brief Parametrized constructor
    Hit(const Int_t& b, const Int_t& ec, const Int_t& c, const Int_t& r, const Int_t& tot, const Int_t& toa, const Int_t& cal);
    /// @brief Destructor
    virtual ~Hit();

    // Setters
    void setBoardNumber(const Int_t& n) { fBoardNumber = (UChar_t)n; }
    void setErrorCode(const Int_t& ec)  { fErrorCode = (Char_t)ec; }
    void setColumn(const Int_t& c)      { fColumn = (UChar_t)c; }
    void setRow(const Int_t& r)         { fRow = (UChar_t)r; }
    void setTot(const Int_t& tot)       { fTot = (UShort_t)tot; }
    void setToa(const Int_t& toa)       { fToa = (UShort_t)toa; }
    void setCal(const Int_t& cal)       { fCal = (UShort_t)cal; }

    // Getters
    Int_t boardNumber() const { return (Int_t)fBoardNumber; }
    Int_t errorCode() const   { return (Int_t)fErrorCode; }
    Int_t column() const      { return (Int_t)fColumn; }
    Int_t row() const         { return (Int_t)fRow; }
    Int_t tot() const         { return (Int_t)fTot; }
    Int_t toa() const         { return (Int_t)fToa; }
    Int_t cal() const         { return (Int_t)fCal; }
 
  private:

    /// @brief Board number
    UChar_t fBoardNumber;
    /// @brief Error code
    Char_t  fErrorCode;
    /// @brief Pixel column number (-1 if disabled)
    Char_t  fColumn;
    /// @brief Pixel row number (-1 if disabled)
    Char_t  fRow;
    /// @brief Time over threshold
    UShort_t fTot;
    /// @brief Time of arrival
    UShort_t fToa;
    /// @brief Charge
    UShort_t fCal;

    ClassDef(Hit, 1)
};

#endif // #define Hit_t