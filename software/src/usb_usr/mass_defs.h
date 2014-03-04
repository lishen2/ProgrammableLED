#ifndef _MASS_DEFS_H_
#define _MASS_DEFS_H_

/******************************************************/
/* SCSI Commands */
/******************************************************/
#define SCSI_FORMAT_UNIT                            0x04
#define SCSI_INQUIRY                                0x12
#define SCSI_MODE_SELECT6                           0x15
#define SCSI_MODE_SELECT10                          0x55
#define SCSI_MODE_SENSE6                            0x1A
#define SCSI_MODE_SENSE10                           0x5A
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1E
#define SCSI_READ6                                  0x08
#define SCSI_READ10                                 0x28
#define SCSI_READ12                                 0xA8
#define SCSI_READ16                                 0x88

#define SCSI_READ_CAPACITY10                        0x25
#define SCSI_READ_CAPACITY16                        0x9E

#define SCSI_REQUEST_SENSE                          0x03
#define SCSI_START_STOP_UNIT                        0x1B
#define SCSI_TEST_UNIT_READY                        0x00
#define SCSI_WRITE6                                 0x0A
#define SCSI_WRITE10                                0x2A
#define SCSI_WRITE12                                0xAA
#define SCSI_WRITE16                                0x8A

#define SCSI_VERIFY10                               0x2F
#define SCSI_VERIFY12                               0xAF
#define SCSI_VERIFY16                               0x8F

#define SCSI_SEND_DIAGNOSTIC                        0x1D
#define SCSI_READ_FORMAT_CAPACITIES                 0x23

/********************************************************************/
/* End of SCSI commands */
/********************************************************************/

/********************************************************************/
/* SCSI SENSE CODE */
/********************************************************************/
/*****Sense key****/
#define SCSI_SENSE_KEY_NO_SENSE		                        0
#define SCSI_SENSE_KEY_RECOVERED_ERROR		                1
#define SCSI_SENSE_KEY_NOT_READY		                    2
#define SCSI_SENSE_KEY_MEDIUM_ERROR		                    3
#define SCSI_SENSE_KEY_HARDWARE_ERROR		                4
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST		                5
#define SCSI_SENSE_KEY_UNIT_ATTENTION		                6
#define SCSI_SENSE_KEY_DATA_PROTECT		                    7
#define SCSI_SENSE_KEY_BLANK_CHECK		                    8
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC		                9
#define SCSI_SENSE_KEY_COPY_ABORTED		                    10
#define SCSI_SENSE_KEY_ABORTED_COMMAND		                11
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW		                13
#define SCSI_SENSE_KEY_MISCOMPARE		                    14

/*****ASC****/
#define SCSI_SENSE_ASC_NO_SENSE                                    0x00
#define SCSI_SENSE_ASC_INVALID_COMMAND                             0x20
#define SCSI_SENSE_ASC_INVALID_FIELED_IN_COMMAND                   0x24
#define SCSI_SENSE_ASC_PARAMETER_LIST_LENGTH_ERROR                 0x1A
#define SCSI_SENSE_ASC_INVALID_FIELD_IN_PARAMETER_LIST             0x26
#define SCSI_SENSE_ASC_ADDRESS_OUT_OF_RANGE                        0x21
#define SCSI_SENSE_ASC_MEDIUM_NOT_PRESENT 			               0x3A
#define SCSI_SENSE_ASC_MEDIUM_HAVE_CHANGED			               0x28

/********************************************************************/
/* END OF SCSI SENSE CODE	*/
/********************************************************************/

#define SCSI_CBW_SIGNATURE             0x43425355
#define SCSI_CSW_SIGNATURE             0x53425355
#define SCSI_CBW_PACKET_LENGTH         31

/* CSW Status Definitions */
#define SCSI_CSW_PASSED                0x00
#define SCSI_CSW_FAILED                0x01
#define CSCSI_CSW_PHASE_ERROR          0x02

/* Bulk-only Command Block Wrapper */
typedef struct _Bulk_Only_CBW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
}
MASS_Bulk_Only_CBW;

/* Bulk-only Command Status Wrapper */
typedef struct _Bulk_Only_CSW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
}
MASS_Bulk_Only_CSW;

#endif

