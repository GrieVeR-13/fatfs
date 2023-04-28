/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <stdlib.h>
#include "ff.h"            /* Obtains integer types */
#include "diskio.h"        /* Declarations of disk functions */
#include "cutil/raio.h"
#include "cutil/jniutil.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM        0    /* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC        1    /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB        2    /* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
        jobject raio        /* Physical drive nmuber to identify the drive */
) {
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
        jobject raio                /* Physical drive nmuber to identify the drive */
) {
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
        jobject raio,        /* Physical drive nmuber to identify the drive */
        BYTE *buff,        /* Data buffer to store read data */
        LBA_t sector,    /* Start sector in LBA */
        UINT count        /* Number of sectors to read */
) {
    JNIEnv *env = c_get_env();
    ssize_t res = raio_pread_all(env, raio, (uint8_t *) buff, (size_t) (count * FF_SECTOR_SIZE),
                                 (off64_t) (sector * FF_SECTOR_SIZE));
    if (res < 0) {
        return RES_ERROR;
    }
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
        jobject raio,            /* Physical drive nmuber to identify the drive */
        const BYTE *buff,    /* Data to be written */
        LBA_t sector,        /* Start sector in LBA */
        UINT count            /* Number of sectors to write */
) {

    JNIEnv *env = c_get_env();
    ssize_t res = raio_pwrite(env, raio, (uint8_t *) buff, (size_t) (count * FF_SECTOR_SIZE),
                              (off64_t) (sector * FF_SECTOR_SIZE));
    if (res < 0) {
        return RES_ERROR;
    }
    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

static DRESULT getSectorCount(JNIEnv *env, jobject raio, DWORD *count) {
    off64_t size = raio_get_size(env, raio);
    if (size < 0) {
        return RES_ERROR;
    }
    *count = size / FF_SECTOR_SIZE;
    return RES_OK;
}

DRESULT disk_ioctl(
        jobject raio,        /* Physical drive nmuber (0..) */
        BYTE cmd,        /* Control code */
        void *buff        /* Buffer to send/receive control data */
) {
    int rc;
    JNIEnv *env = c_get_env();
    switch (cmd) {
        case CTRL_SYNC :
            rc = raio_fsync(env, raio);
            if (rc < 0) {
                return RES_ERROR;
            }
            return RES_OK;
        case GET_SECTOR_COUNT :
            return getSectorCount(env, raio, (DWORD *) buff);
        case GET_BLOCK_SIZE :
            return 1; //unknown
        case GET_SECTOR_SIZE :
        case CTRL_TRIM :
            abort();
    }
    abort();
}

