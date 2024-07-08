/*********************/
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
/*********************/

using namespace std;

/******************** Helper Functions *********************/
string to_binary( char in ){
    string res ; // convert char to string of 0's and 1's
    res += ( (bool)( ( char )128 & in )  + 48 )  ; // MSB
    res += ( (bool)( ( char )64  & in )  + 48 )  ;
    res += ( (bool)( ( char )32  & in )  + 48 )  ;
    res += ( (bool)( ( char )16  & in )  + 48 )  ;
    res += ( (bool)( ( char )8   & in )  + 48 )  ;
    res += ( (bool)( ( char )4   & in )  + 48 )  ;
    res += ( (bool)( ( char )2   & in )  + 48 )  ;
    res += ( (bool)( ( char )1   & in )  + 48 )  ; // LSB
    return res ;
}
char to_char( string in ){
    char res = 0 ; // convert char to string of 0's and 1's
    res += 128 * ( in[ 0 ] - 48 )  ; // MSB
    res += 64  * ( in[ 1 ] - 48 )  ;
    res += 32  * ( in[ 2 ] - 48 )  ;
    res += 16  * ( in[ 3 ] - 48 )  ;
    res += 8   * ( in[ 4 ] - 48 )  ;
    res += 4   * ( in[ 5 ] - 48 )  ;
    res += 2   * ( in[ 6 ] - 48 )  ;
    res += 1   * ( in[ 7 ] - 48 )  ; // LSB
    return res ;
}
/**********************************************************/

/*************** BBS RANDOM KEY GENERATOR ****************/
string Key_Generator(){
    string key ; // 128 Byte -> 1024 bit
    int q = 2207 , p = 3571 ; // two prime numbers = 3 % 4
    int n = p * q ;
    int s = 0 ; // seed
    while( s % q == 0 || s % p == 0 ) s = rand() % n ; // select seed relatively prime to p and q
    for( int i = 0 ; i < 128 ; i++ ){ // 128 iterations ( one for each Byte )
        unsigned char current_8b = 0 ; // current Byte of key ( 8 bit )
        int weight = 1 ; // used to set the value of the current bit by the weight
        for( int j = 0 ; j < 8 ; j++ ){ // 8 iterations ( one for each bit )
            int x = (s*s) % n ; // generator
            current_8b += ( x%2 * weight ) ;
            s = x ;
            weight *= 2 ; // to get the next bit
        }
        key += current_8b ; // add new Byte to the key
    }
    return key ; // return randomly generated key
}
void Uniform_Distribution_Test( string key ){
    /**** TEST 1 ***/
    int ones = 0 ; // counter for 0's
    int zeros = 0 ; // counter for 1's
    for( int i = 0 ; i < 128 ; i++  ){
       unsigned char weight = 1 ; // used to determine the value of current bit by the weight of it
       for( int j = 0 ; j < 8 ; j++ ){
        char check = key[ i ] & weight ; // check if 0
        if( check == 0 ) zeros++ ;
        else ones++ ;
        weight *= 2 ; // to get the next bit
       }
    }
    cout<<" number of zeros in key : "<<zeros<<" ( "<<( float )( zeros / 1024.0 * 100 )<<"% )"<<endl ; // print the percentage of zeros
    cout<<" number of ones  in key : "<<ones<<" ( "<<( float )( ones / 1024.0 * 100 )<<"% )"<<endl ; // print the percentage of ones
    cout<<"\n\n" ;

    /**** TEST 2 ***/
    for( int k = 1 ; k <= 3 ; k++ ){ // for the second test
        unsigned char pattern = rand() % 256 ; // select k'th 8 bit pattern randomly
        int counter = 0 ; // counter to the occurrences of pattern k
        unsigned char current = key[ 0 ] ; // store the current Byte to compare with
        if( pattern == current ) counter++ ;
        for( int i = 1 ; i < 128 ; i++ ){ // traverse through the rest of the key bits
                int weight = 1 ; // to get the current bit
            for( int j = 0 ; j < 8 ; j++ ){
                current /= 2 ; // delete the LSB
                current += 128*( bool(weight & key[ i ]) ) ; // add the current bit as MSB
                if( pattern == current ) counter ++ ; // increment the counter if pattern founded
                weight *= 2 ;
            }
        }
        cout<<" pattern "<< k <<" : " ;
        int weight = 128 ; // start from MSB
        for( int i = 0 ; i < 8 ; i++ ){
            cout<<bool( weight & pattern )<<" "; // print the pattern in binary
            weight /= 2 ;
        }
        cout<<" occurs "<<counter<<" times "<<endl; // print the number of occurrences
    }
}
/********************************************************/

/*********************  RC4 ****************************/
string Text_Encryption_or_Decryption( string key , string plain ){
      string cipher = plain ;
      unsigned char S[ 256 ] ; // internal state
      unsigned char T[ 256 ] ; // copies the key twice
      for( int i = 0 ; i < 256 ; i++ ){
        S[ i ] = i ; // initial value
        T[ i ] = key[ i % 128 ] ; // copying the key in T
      }
      int j = 0 ;
      for( int i = 0 ; i < 256 ; i++ ){
        j = ( j + S[ i ] + T[ i ] ) % 256 ; // shuffling the S array using T and S itself
        swap( S[ i ] , S[ j ] ) ; // the initial permutation of S
      }
      int i = 0 ;
      j = 0 ;
      for( int counter = 0 ; counter < plain.size() ; counter++ ){
        i = ( i + 1 ) % 256 ;
        j = ( j + S[ i ] ) % 256 ;
        swap( S[ i ] , S[ j ] ) ; // second permutation
        int t = ( S[ i ] + S[ j ] ) % 256 ;
        cipher[ counter ] = plain[ counter ] ^ S[ t ] ; // encryption using XOR
      }
      return cipher ;
}
/******************************************************/
int main()
{
    srand( time( 0 ) ) ; // Time Seed
    string key = Key_Generator() ; // get the key
    Uniform_Distribution_Test( key ) ; // apply tests on the key
    cout<<"\n\n" ;
    /***************** program interface ************************/
    while( 1 ){
        char state ;
        cout<<" E ( Encrypt ) , D ( Decrypt ) , Q( Quit ) : " ;
        cin >> state ;
        if( state != 'D' && state != 'E' )
        {
            cout<<"\nEnding program" ;
            break ;
        }
        string if_name, of_name; // to store the names of the files
        cout<<" Input File name : " ;
        cin >> if_name ; if_name += ".txt" ;
        ifstream input( if_name ) ; // input text file
        string in_txt;
        char ch ;
        while( input.get(ch) ) in_txt += ch ; // store input text on string
        cout<<" Output File name : " ;
        cin >> of_name ; of_name += ".txt" ;
        string result ;
        cout<<"\n\n" ;
        switch( state )
        {
        case 'E' :
            {
            string temp ;
            temp = Text_Encryption_or_Decryption( key , in_txt ); // save Encrypted text on string
            for( int  i = 0 ; i < in_txt.size() ; i++ ) result += to_binary( temp [ i ] ) ; // convert on binary form to save on file
            break ;
            }
        case 'D' :
            {
            string temp ;
            for( int i = 0 ; i < in_txt.size() ; i += 8 )temp += to_char( in_txt.substr( i , 8 ) ) ; // convert input file to chars
            result = Text_Encryption_or_Decryption( key , temp ) ; // Decryption process
            break ;
            }
        }
        ofstream output( of_name ) ; // generate the output file
        for( int i = 0 ; i < result.size() ; i++ )
            output.put( result[ i ] ) ; // store the result of Encryption or Decryption on it
    }
    /************************************************************/
    return 0 ;
}

