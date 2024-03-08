
namespace ww_url {

  int8_t s8_int_from_char( char* pac_in );
  String s_string_from_int( int8_t s8_in );
  String s_url_from_page_and_client( int8_t s8_page, int8_t s8_client );
  bool b_base_url( char* pac_url );
  int8_t s8_from_url( char* pac_url, char c_before, char c_after );
  int8_t s8_this_from_url( char* pac_url );
  int8_t s8_client_from_url( char* pac_url );
}
