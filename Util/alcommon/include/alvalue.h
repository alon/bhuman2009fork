/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_VALUE_H
#define AL_VALUE_H

//! Small function and tools ready to move to a "global tools module"
//#pragma warning( disable: 4251 ) // remove warning

#include <cstdio>
#include <ostream>
#include "altypes.h"
#include "altools.h"
#include "alerror.h"
#include "alptr.h"


#ifndef FINAL_RELEASE
#   define AL_VALUE_ASSERT(b) {if( !(b) ) { throw ALERROR( "ALValue", "Assert failed", std::string( __FILE__ ) + ":" + DecToString( __LINE__ ) ); } }
    // Alma 08-01-15: choisir la bonne option pour activer ou pas les traces de cette classe
    // #define AL_VALUE_DEBUG(...) printf(__VA_ARGS__)
    #define AL_VALUE_DEBUG(...)

#else
# define AL_VALUE_ASSERT(b) /* b */
# define AL_VALUE_DEBUG(...)
#endif

struct soap;


namespace AL
{

  // Definition of type for SOAP communication

  // definition of base type used in ALValue
  typedef int           TALValueInt;
  typedef float         TALValueFloat;
  typedef double        TALValueDouble;
  typedef bool          TALValueBool;
  typedef std::string   TALValueString;


  class  ALValue;

  typedef std::vector< unsigned char >				TAlValueBinaryData;
  typedef TAlValueBinaryData::iterator				ITAlValueBinaryData; // Alma 08-01-15: Soap don't like this -> moved to the altypes.h
  typedef TAlValueBinaryData::const_iterator 	CITAlValueBinaryData;

  typedef std::vector<ALValue>						    TAlValueArray;
  typedef TAlValueArray::iterator				      ITAlValueArray;
  typedef TAlValueArray::const_iterator	      CITAlValueArray;



  union unionValue
  {
    TAlValueArray * 		  asArray;
    TALValueBool					asBool;
    TALValueInt						asInt;
    TALValueFloat					asFloat;	// internaly we only manafe float even if call use double
    TALValueString *		  asString;
    TAlValueBinaryData *  asBinary;
  };

  /**
  * Implementation of the ALdebaran Value. \n
  * A complex variant object to store and communicate parameters.
  * Because it's used by SOAP, some functions or variable will used unconventionnal namming.
  *1
  * To generate SOAP file, d:\dev\naoqi0.2\extern\c\ALDEBA~1\alcommon\interface> c:\gsoap-win32-2.7\bin\soapcpp2.exe I_ALBroker.h
  * and add around line 57 in stub.h, this line:
  * class ALValue; // Alma 08-01-15: Add manually this line in SOAP generated file to compile the naoqi project.
  *
  */



  class ALValue/* : public AL::AnyPtr<ALValue>*/
  {
    friend void ConvertALValueToSoapXml( const ALValue* pAlValue, std::string& strToFill, bool bInAnArray );
    friend std::ostream &operator<<( std::ostream& os, const ALValue& a );

  public:
    //! ASSUME same order than SOAP_UNION_unionValue_asArray ...
    enum Type
    {
      TypeInvalid = 0,
      TypeArray,
      TypeBool,
      TypeInt,
      TypeFloat,
      TypeString,
      TypeBinary, // = TypeBase64
    };

  public:

    //! Constructors
    ALValue();
    ALValue(TALValueBool value);
    ALValue(int value);
    ALValue(TALValueDouble value);
    ALValue(TALValueFloat value);

    ALValue(TALValueString const& value);

    ALValue(const char* value);

    ALValue(const void* value, int nBytes);

    ALValue(const TAlValueBinaryData& rhs);

    // constructor from stl array
    ALValue(const TStringArray& pListString );
    ALValue(const TFloatArray& pListFloat );
    ALValue(const TIntArray& pListInt );

    // constructor from standard array
    ALValue(const char** pArrayOfString, int nNbrString );
    ALValue(const float* pFloat, int nNbrElement );
    ALValue(const int* pInt, int nNbrElement );

    //! Copy
    ALValue(ALValue const& rhs);

    /**
    * Default destructor.
    * (make virtual if you want to subclass)
    */
    virtual ~ALValue();

    //! Erase the current value (reset)
    void clear() { xInvalidate(); }

    //! Operators
    ALValue& operator=(ALValue const& rhs);
    ALValue& operator=(TALValueBool const& rhs);
    ALValue& operator=(int const& rhs)
    {
      AL_VALUE_DEBUG( "ALValue& AL Value::operator=(int const& rhs)\n" );

      // return operator=(ALValue(val));   // optimal en ligne de code, mais pas a l'execution
      if( !isInt() )
      {
        xInvalidate();
        __union = TypeInt;
      }
      fValue.asInt = (int)rhs;
      return *this;
    }
    ALValue& operator=(TALValueDouble const& rhs);
    ALValue& operator=(TALValueFloat const& rhs)
    {
      AL_VALUE_DEBUG( "ALValue& ALValue::operator=(double const& rhs)\n" );
      // return operator=(ALValue((float)rFloat)); // optimal en ligne de code, mais pas a l'execution
      if( !isFloat() )
      {
        xInvalidate();
        __union = TypeFloat;
      }
      fValue.asFloat = rhs;
      return *this;
    }

    ALValue& operator=(const char* rhs);
    ALValue& operator=(const TAlValueBinaryData& rhs);
    ALValue& SetBinary(const void* rhs, int nDataSizeInBytes );

    //ALValue& operator=(void); // not implemented

    bool operator==(ALValue const& other) const;
    bool operator!=(ALValue const& other) const
    {
      return !(*this == other);
    }


    //! Conversion operator
    operator TALValueBool&();
    operator const TALValueBool&() const;
    operator int&()
    {
      xAssertCompatibleType( TypeInt );
      return fValue.asInt;
    }

    // For remote/local compatibilities on bound function that use ALPtr<T>
     template <typename T>
    operator ALPtr<T>()
    {
      return ALPtr<T>();
    }



    operator int() const
    {
      if( getType() == TypeInt )
      {
        return fValue.asInt;
      }
      // Alma 08-03-11: Adding acceptable conversion
      if( getType() == TypeBool )
      {
        return (int)fValue.asBool;
      }
      // else: throw
      xAssertType( TypeInt );
      return 0;
    }
    operator TALValueDouble() const;
    operator TALValueFloat&()
    {
      AL_VALUE_DEBUG( "ALValue::operator TALValueFloat&()\n" );
      // EPOT 09/04/2009 : When we do a remote call, and call this cast,
      // we can safely convert an int to a float.
      if( getType() == TypeInt )
      {
        // we cast the ALValue to a float, as it is possible
        fValue.asFloat = (TALValueFloat)fValue.asInt;
        __union = TypeFloat;
      }
      xAssertCompatibleType( TypeFloat );
      return fValue.asFloat;
    }
    operator TALValueFloat() const
    {
      AL_VALUE_DEBUG( "ALValue::operator TALValueFloat()\n" );
      if( getType() == TypeFloat )
      {
        return fValue.asFloat;
      }
      // Alma 08-02-12: Adding acceptable conversion
      if( getType() == TypeInt )
      {
        return (float)( fValue.asInt );
      }
      // else: throw
      xAssertType( TypeFloat );
      return 0.f;

    }
    operator TALValueString&();
    operator const TALValueString&() const;
    // operator const char*() const; // not implemented
    operator TAlValueBinaryData&();
    operator const TAlValueBinaryData&() const;
    operator const void*() const;
    const void*	GetBinary() const; // Explicit operator to convert const void*

    operator TStringArray() const;
    operator TFloatArray() const;
    operator TIntArray() const;

// explicit convertion operator


    // explicit convertion operator
    template<typename T>
    T Convert( void ) const
    {
      T valTemp = *this;
      return valTemp;
    }

    // optimisation in case of ALValue->ALValue
#ifdef WIN32
    template<>
#endif
    ALValue Convert( void ) const // Reflexion: le compilo ne va t'il pas quand meme copier l'objet ? clarifirions nous le truc en retournant un "const ALValue &" ?
    {
      return *this;
    }


    /*
    * ToStringArray explicit convert to vector<string>
    *
    * @param pArrayToFill the value to fill with string value from the ALValue
    * @param bInsertDefaultOnError a flag to inform what to do on a not wanted type. If set to false, invalid value are skipped, on true empty string will be inserted in the vector
    *
    */
    void ToStringArray( TStringArray& pArrayToFill, bool bInsertDefaultOnError = false ) const;
    // Explicit operator to convert to vector<float>
    void ToFloatArray( TFloatArray& pArrayToFill, bool bInsertDefaultOnError = false ) const;
    // Explicit operator to convert to vector<int>
    void ToIntArray( TIntArray& pArrayToFill, bool bInsertDefaultOnError = false ) const;


    ALValue& operator[](int i)
    {
      AL_VALUE_DEBUG( "ALValue::operator []\n" );
      xAssertCompatibleType( TypeArray );
      xAssertArraySize( i+1 );
      return fValue.asArray->at(i);
    }
    const ALValue& operator[](int i) const
    {
      AL_VALUE_DEBUG( "ALValue::operator [] const\n" );
      xAssertType( TypeArray );
      xAssertArraySize( i+1 );
      return fValue.asArray->at(i);
    }
    // Accessors
    //! Return true if the value has been set.
    bool isValid() const { return __union != TypeInvalid; }

    //! Return the type of the value stored. \see Type.
    enum Type getType() const { return (ALValue::Type)__union; }

    bool isArray () const { return getType() == TypeArray; }
    bool isBool  () const { return getType() == TypeBool; }
    bool isInt   () const { return getType() == TypeInt; }
    bool isFloat () const { return getType() == TypeFloat; }
    bool isString() const { return getType() == TypeString; }
    bool isBinary() const { return getType() == TypeBinary; }

    //! Return the size for string, base64 and array values.
    unsigned int getSize() const
    {
      switch (__union)
      {
      case TypeString: AL_VALUE_ASSERT( fValue.asString ); return (int)fValue.asString->size();
      case TypeBinary: AL_VALUE_ASSERT( fValue.asBinary ); return (int)fValue.asBinary->size();
      case TypeArray:  AL_VALUE_ASSERT( fValue.asArray  ); return (int)fValue.asArray->size();
      case TypeInvalid:  return 0;
      default: break;
      }
      return 0;
    }

    //! Specify the size the array values will use.
    //! (Array values will grow beyond this size if needed)
    void arrayReserve(int size)
    {
      xAssertCompatibleType( TypeArray );
      fValue.asArray->reserve( size );
    }

    //! Specify the used size for array values.
    void arraySetSize(int size)
    {
      xAssertCompatibleType( TypeArray ); // We make this test to test it and to translate to array type if required

      // Alma 08-02-11: dans tout les cas, il faut resizer, meme si ca doit reduire le vecteur.
      // if( int(fValue.asArray->size()) < pnSize )
      {
        fValue.asArray->resize( size );
      }
    }

    //! Push a new value in the array
    void arrayPush( const ALValue& pSrcToCopyNotOwned );

    //! Pop the last value used TODO, question: erase first one or last one ? +  erase the popped one?
    ALValue arrayPop();

    /**
    * Return a string describing this object, for debug purpose.
    * @param pnVerbosity style of ouput format
    * WARNING: the VerbosityMini style is used in some code, it's the minimal description of the contents of an alvalue, with [] to describe array.
    * WARNING:          it's done to be directly evaluatable in script language (tested with: urbi and ruby)
    */
    std::string toString( Verbosity pnVerbosity = VerbosityMedium ) const;

    //-----------------------------------------------------------------------------
    //  Name  :  decodeB64
    /**
    * @param pszB64 the buffer to decode
    * @return true if ok
    *
    * Object : Decode a string containing some binaries encoded in B64 (binary 64 / uuencoded)
    * Warning: public method called from FillAlValueFromXmlNodeContent...
    *
    */
    // Created: Alma 08-01-24
    //-----------------------------------------------------------------------------
    bool decodeB64( const char* pszB64 );

    //-----------------------------------------------------------------------------
    //  Name  :  encodeB64
    /**
    * @param strOutput the buffer to receive or data encoded in B64
    * @return  true if ok
    *
    * Object : Decode a string containing some binaries encoded in B64 (binary 64 / uuencoded)
    *
    */
    // Created: Alma 08-01-24
    //-----------------------------------------------------------------------------
    void encodeB64( std::string& strOutput ) const;

    //-----------------------------------------------------------------------------
    //  Name  :  deduceType
    /**
    * @param szInput a text to analyse, eg: "1", "toto", or "[36,3,"toto",3.5,[1,2,3]]"
    * @param nLimitToLen limit analysis to <nLimitToLen> first characters
    * @return  true if ok
    *
    * Object : Analyses a string and try to guess the type of data describe in the string.
    *          eg: "5" => int; "3.5" => float; "'toto'" => string; [3,5] => array ...
    *
    */
    // Created: Alma 08-02-02
    //-----------------------------------------------------------------------------
    static enum Type deduceType( const char* szInput, int nLimitToLen = 0x7FFFFFFF );

    //-----------------------------------------------------------------------------
    //  Name  :  unSerializeFromText
    /**
    * @param szInput the source analysed to construct the alvalue, eg: "1", "toto", or "[36,3,"toto",3.5,[1,2,3]]"
    * @param nLimitToLen limit analysis to <nLimitToLen> first characters (used in recursion)
    * @return  true if ok
    *
    * Object : Create an ALValue from a text
    *
    */
    // Created: Alma 09-02-02
    //-----------------------------------------------------------------------------
    void unSerializeFromText( const char* szInput, int nLimitToLen = 0x7FFFFFFF );

    //-----------------------------------------------------------------------------
    //  Name  :  serializeToText
    /**
    * @return  the new string
    *
    * Object : Create a text describing an alvalue
    *
    */
    // Created: Alma 09-02-03
    //-----------------------------------------------------------------------------
    std::string serializeToText( void ) const;

    //-----------------------------------------------------------------------------
    //  Name  :  xUnSerializeFromText_InnerTest
    /**
    *
    * Object : internal function to validate the correct functionnality of unSerializeFromText
    * @return  true if ok
    */
    // Created: Alma 09-02-02
    //-----------------------------------------------------------------------------
    //
    static bool xUnSerializeFromText_InnerTest( void );

  protected:
    //! Clean up the object
    void xInvalidate();

    //Type checking

    //! Assert the object is compatible with a wanted type.
    //! If the object is unitialiazed, it will be changed to the good type
    void xAssertCompatibleType( enum Type pnWantedType );

    //! Assert the object is compatible has the good type.
    //! If the object is unitialiazed, it will assert !
    void xAssertType( enum Type pnWantedType ) const;

    // Size checking
    //! Assert this object is a vector and has at least pnSize really used element.
    void xAssertArraySize( int pnSize ) const;

    void assertStruct(); // deprecated, use xAssertStruct


    //
    // Type tag and values
    //

    int __union; // a value to help soap to serialize/unserialize data. Seem's to be not renamable
    union unionValue fValue; // the place to store our value
    struct soap* soap; // ! pointer to current gSOAP environment that created this object



    // Manage multipart message (SOAP, CORBA...)
  public:
    int 							fmultiPart_bWaitForFollowingPart; // are we waiting some following parts?
    std::vector<char>	fmultiPart_totalBufferConstructed; // the total buffer constructed from all parts
    int								fmultiPart_nBeginAnalyseTo;	// the begin in the total part where we have to analyse to parse xml

    static std::string TypeToString( enum Type pnType );

  }; // class ALValue

}; // namespace AL

using AL::ALValue; // to help buddies

std::ostream &operator<<( std::ostream& os, const AL::ALValue& a );

/*****************************/
/* Helper related to ALValue */
/*****************************/

/*
* HelperExtractPair extract a pair <string>, <value> from an ALValue.
* \t Standard construction are [0] = string and [1] = value
* \t but others light form are accepted:
*	\t\t [0] = string and no [1]
*	\t\t direct string (not in an array)
*
* @param pSrc the ALValue to analyse
* @param pstrName the value to fill with string value from the ALValue
* @param pValue the value to extract from an int or a float (not modified is not set in the ALValue)
*
*/
template <typename T> void HelperExtractPair( const AL::ALValue& pSrc, const std::string * & pstrName, T& pValue ) // "std::string * &" looks weird but we really want to have a reference on a const*
{

  if( pSrc.isArray() )
  {
    pstrName = & ( (const std::string&) ( pSrc[0] ) );
    if( pSrc.getSize() > 1 )
    {
      pValue = (T)( pSrc[1] );
    }
  }
  else if( pSrc.isString() )
  {
    pstrName = & ( (const std::string&) ( pSrc ) );
  }
  else
  {
    printf( "ERROR: HelperExtractPair: ALValue has a wrong type (not an array nor string): %s\n", pSrc.toString().c_str() );
  }

}

#endif
