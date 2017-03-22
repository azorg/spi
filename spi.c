/*
 * Simple Linux wrapper for access to /dev/spidev
 * File: "spi.c"
 */

//-----------------------------------------------------------------------------
#include "spi.h"
//-----------------------------------------------------------------------------
#include <unistd.h>    // close()
#include <string.h>    // memset()
#include <fcntl.h>     // open()
#include <sys/ioctl.h> // ioctl()
//----------------------------------------------------------------------------
// open and init SPIdev
// * spi_mode may have next mask: SPI_LOOP | SPI_CPHA | SPI_CPOL |
//   SPI_LSB_FIRST | SPI_CS_HIGH SPI_3WIRE | SPI_NO_CS | SPI_READY
//   or 0 by zefault
int spi_init(spi_t *self,
             const char *device, // filename like "/dev/spidev0.0"
             int mode,           // SPI_* (look "linux/spi/spidev.h")
             int bits,           // bits per word (usually 8)
             int speed)          // max speed [Hz]
{
  // zero fill `struct spi_ioc_transfer xfer[2]`
  memset(&self->xfer, 0, sizeof(self->xfer));

  // open SPIdev
  self->fd = open(device, O_RDWR);
  if (self->fd < 0)
  {
    SPI_DBG("error in spi_init(): failed to open the bus");
    return SPI_ERR_OPEN;
  }

  // set mode
  if (mode)
  {
    self->mode = (__u8) mode;
    if (ioctl(self->fd, SPI_IOC_WR_MODE, &self->mode) < 0)
    {
      SPI_DBG("error in spi_init(): can't set bus mode");
      return SPI_ERR_SET_MODE;
    }
  }

  // get mode
  if (ioctl(self->fd, SPI_IOC_RD_MODE, &self->mode) < 0)
  {
    SPI_DBG("error in spi_init(): can't get bus mode");
    return SPI_ERR_GET_MODE;
  }

  // get "LSB first"
  if (ioctl(self->fd, SPI_IOC_RD_LSB_FIRST, &self->lsb) < 0)
  {
    SPI_DBG("error in spi_init(): can't get 'LSB first'");
    return SPI_ERR_GET_LSB;
  }

  // set bits per word
  if (bits)
  {
    self->bits = (__u8) bits;
    if (ioctl(self->fd, SPI_IOC_WR_BITS_PER_WORD, &self->bits) < 0)  
    {
      SPI_DBG("error in spi_init(): can't set bits per word");
      return SPI_ERR_SET_BITS;
    }
  }

  // get bits per word
  if (ioctl(self->fd, SPI_IOC_RD_BITS_PER_WORD, &self->bits) < 0) 
  {
    SPI_DBG("error in spi_init(): can't get bits per word");
    return SPI_ERR_GET_BITS;
  }

  // set max speed [Hz]
  if (speed)
  {
    self->speed = (__u32) speed;
    if (ioctl(self->fd, SPI_IOC_WR_MAX_SPEED_HZ, &self->speed) < 0)  
    {
      SPI_DBG("error in spi_init(): can't set max speed [Hz]");
      return SPI_ERR_SET_SPEED;
    }
  }
  
  // get max speed [Hz]
  if (ioctl(self->fd, SPI_IOC_RD_MAX_SPEED_HZ, &self->speed) < 0)  
  {
    SPI_DBG("error in spi_init(): can't get max speed [Hz]");
    return SPI_ERR_SET_SPEED;
  }

  SPI_DBG("open device='%s' mode=%d bits=%d lsb=%d max_speed=%d [Hz]",
          device, (int)self->mode, (int)self->bits, (int)self->lsb,
          (int)self->speed);

  return SPI_ERR_NONE;
}
//----------------------------------------------------------------------------
// close SPIdev file and free memory
void spi_free(spi_t *self)
{
  close(self->fd);
}
//----------------------------------------------------------------------------
// read data from SPIdev
int spi_read(spi_t *self, char *rx_buf, int len)
{
  int retv;

  self->xfer.tx_buf = (__u64) 0;      // output buffer
  self->xfer.rx_buf = (__u64) rx_buf; // input buffer
  self->xfer.len    = (__u32) len;    // length of data to read

  retv = ioctl(self->fd, SPI_IOC_MESSAGE(1), &self->xfer);
  if (retv < 0)
  {
    SPI_DBG("error in spi_read(): ioctl(SPI_IOC_MESSAGE(1)) return %d", retv);
    return SPI_ERR_READ;
  }

  return retv;
}
//----------------------------------------------------------------------------
// write data to SPIdev
int spi_write(spi_t *self, const char *tx_buf, int len)
{
  int retv;

  self->xfer.tx_buf = (__u64) tx_buf; // output buffer
  self->xfer.rx_buf = (__u64) 0;      // input buffer
  self->xfer.len    = (__u32) len;    // length of data to write

  retv = ioctl(self->fd, SPI_IOC_MESSAGE(1), &self->xfer);
  if (retv < 0)
  {
    SPI_DBG("error in spi_write(): ioctl(SPI_IOC_MESSAGE(1)) return %d", retv);
    return SPI_ERR_WRITE;
  }

  return retv;
}
//----------------------------------------------------------------------------
// read and write `len` bytes from/to SPIdev
int spi_exchange(spi_t *self, char *rx_buf, const char *tx_buf, int len)
{
  int retv;

  self->xfer.tx_buf = (__u64) tx_buf; // output buffer
  self->xfer.rx_buf = (__u64) rx_buf; // input buffer
  self->xfer.len    = (__u32) len;    // length of data to write

  retv = ioctl(self->fd, SPI_IOC_MESSAGE(1), &self->xfer);
  if (retv < 0)
  {
    SPI_DBG("error in spi_exchange(): ioctl(SPI_IOC_MESSAGE(1)) return %d",
             retv);
    return SPI_ERR_EXCHANGE;
  }

  return retv;
}
//----------------------------------------------------------------------------

/*** end of "spi.c" file ***/


