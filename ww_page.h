#ifndef PAGE
  #define PAGE

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

class ww_page {

  public:

// Instance constructor polymorphisms.
    
    ww_page( char* pac_html );
    ww_page( char* pac_html, perspective_t e_perspective );
    static void v_begin( );

// Class variables.

    static ww_page* ap_page_list[ PAGE_BOUND + 1 ]; // PAGE_BOUND is the index of the last page.

    static char ac_splash[];
    static char ac_html_00[];
    static char ac_html_01[];
    static char ac_html_02[];
    static char ac_html_03[];
    static char ac_html_04[];

// Instance variables.

    perspective_t e_perspective;
    char ac_html[ HTML_BOUND + 1 ];
    char* pac_html = ac_html;

};

#endif
