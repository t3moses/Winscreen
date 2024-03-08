
#include "ww_types.h"
#include "ww_constants.h"
#include "ww_storage.h"
#include <nvs_flash.h>
#include <nvs.h>

char ac_namespace[] = "WW";
char* pac_namespace = ac_namespace;

char ac_orh_key[] = "OFFSET";
char* pac_orh_key = ac_orh_key;

char ac_mrn_key[] = "VARIATION";
char* pac_mrn_key = ac_mrn_key;

char ac_fc_key[] = "FC";
char* pac_fc_key = ac_fc_key;



void ww_storage::v_begin( correction_data_t* p_correction_data ) {

 //
 // Initialize non-volatile storage.
 // Get a handle for the namespace.
 // Check if the key already exists.  If it does NOT, create it by setting the initial values.
 // If it does exist, do NOT modify the existing values.
 // Restore correction data from non-volatile storage.
 //

nvs_handle_t storage_handle;

int16_t s16_orh; // Windvane offset in degrees.
int16_t s16_mrn; // Compass variation in degrees.
int16_t s16_fc; // Index into the ad_fc[ ] array of doubles in Hz.

  nvs_flash_init();
  nvs_open( pac_namespace, NVS_READWRITE, &storage_handle );

// Check if the correction data has been programmed already by attempting to read windvane offset.

  esp_err_t err = nvs_get_i16( storage_handle, pac_orh_key, &s16_orh );

  if( err == ESP_ERR_NVS_NOT_FOUND ) {

// The attempt to read windvane offset failes.
// So, this is a run before any values have been stored.
// So, store the default values.

    nvs_set_i16( storage_handle, pac_orh_key, INITIAL_AW_OFFSET );
    nvs_set_i16( storage_handle, pac_mrn_key, INITIAL_MRN );
    nvs_set_i16( storage_handle, pac_fc_key, INITIAL_FC_INDEX );
    nvs_commit( storage_handle );

  }
  
  nvs_close( storage_handle );

// Retrieve and return the stored correction data.

  nvs_open( pac_namespace, NVS_READWRITE, &storage_handle );

  nvs_get_i16( storage_handle, pac_orh_key, &s16_orh );
  nvs_get_i16( storage_handle, pac_mrn_key, &s16_mrn );
  nvs_get_i16( storage_handle, pac_fc_key, &s16_fc );

  p_correction_data->s16_orh = s16_orh;
  p_correction_data->s16_mrn = s16_mrn;
  p_correction_data->s16_fc = s16_fc;

  nvs_close( storage_handle );

}



void ww_storage::v_store( correction_data_t correction_data ) {

nvs_handle_t storage_handle;

  nvs_open( pac_namespace, NVS_READWRITE, &storage_handle );

  nvs_set_i16( storage_handle, pac_orh_key, correction_data.s16_orh );
  nvs_set_i16( storage_handle, pac_mrn_key, correction_data.s16_mrn );
  nvs_set_i16( storage_handle, pac_fc_key, correction_data.s16_fc );
  
  nvs_commit( storage_handle );  
  nvs_close( storage_handle );

}

