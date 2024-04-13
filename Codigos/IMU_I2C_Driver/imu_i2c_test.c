#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>


typedef enum{
    API_OK = 0,
    API_ERR = -1
}retType;

uint8_t buf[255];
uint8_t buf_tmp[255];

#define API_OK 0
#define API_ERR -1

#define MPU9250_WHOIAM 0x75
#define MPU9250_TEMP_H 0x41
#define MPU9250_TEMP_L 0x42
#define MPU9250_ACCEL_XOUT_H 0x3B
#define MPU9250_ACCEL_XOUT_L 0x3C
#define MPU9250_ACCEL_YOUT_H 0x3D
#define MPU9250_ACCEL_YOUT_L 0x3E
#define MPU9250_ACCEL_ZOUT_H 0x3F
#define MPU9250_ACCEL_ZOUT_L 0x40
#define MPU9250_ACCEL_TEMP_OUT_H 0x41
#define MPU9250_ACCEL_TEMP_OUT_L 0x42


#define MPU9250_XA_OFFSET_L 0x78

#define ONE_BYTE 1

#define DEVICE "/dev/mse00"

/* Declaracion de TESTS*/
int TEST_general(void);
int TEST_API_read_byte(void);
int TEST_API_read_bytes(void);
int TEST_API_write_byte(void);

/* Declaracion de API's */
retType i2c_read_byte(int fd, uint8_t reg, uint8_t *buf);
retType i2c_read_bytes(int fd, uint8_t reg, uint8_t *buf, uint8_t len);
retType i2c_write_byte(int fd, uint8_t reg, uint8_t data);

int main(void)
{
    TEST_general();
    TEST_API_read_byte();
    TEST_API_read_bytes();
    TEST_API_write_byte();
    return 0;
}


int TEST_general(void)
{
    int i2c_dev = open(DEVICE, O_RDWR);
    int ret = 0;

    if (i2c_dev < 0)
    {
        perror("Fail to open device file: /dev/mse00\n");
    }
    else
    {
        uint8_t buffer_1[] = {  MPU9250_ACCEL_XOUT_H, 
                                MPU9250_ACCEL_XOUT_L,
                                MPU9250_ACCEL_YOUT_H,
                                MPU9250_ACCEL_YOUT_L,
                                MPU9250_ACCEL_ZOUT_H,
                                MPU9250_ACCEL_ZOUT_L,
                                MPU9250_ACCEL_TEMP_OUT_H,
                                MPU9250_ACCEL_TEMP_OUT_L,
                             };
        uint8_t tmpaddr = 0;

        printf("\n###Test : Lectura byte por byte\n\n");
        for (uint8_t i = 0; i < sizeof(buffer_1)/sizeof(buffer_1[0]); i++)
        {   
            tmpaddr = buffer_1[i];
            ret = read(i2c_dev, &buffer_1[i], ONE_BYTE);
            if (ret < 0)
            {
                printf("No se pudo leer el registro satisfactoriamente!\n");
                goto close_dev;
            }

            printf("Addr %#x -> Hex : %#x\n",tmpaddr, buffer_1[i]);
        }


        /* Agrego al buffer el primer registro desde comienza a contar */
        uint8_t s_addr = MPU9250_ACCEL_XOUT_H;
        buf[0] = s_addr;
        uint16_t len = 8;
        printf("\n###Test : Lectura chunck de %d bytes desde %#x\n\n",len,s_addr);
        ret = read(i2c_dev, buf, len);
        if (ret < 0)
        {
            printf("No se pudo leer %d registros satisfactoriamente!\n",len);
            goto close_dev;
        }

        for (uint8_t i = 0; i < len; i++)
        {
            printf("Addr %#x -> Hex : %#x\n",s_addr + i, buf[i]);
        }

close_dev:
        printf("\nSe cierra fd\n");
        close(i2c_dev);
    }

    return 0;
}

int TEST_API_write_byte(void)
{
    printf("\n###Test : API_write_byte \n\n");
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        printf("Error abriendo fd");
    }
    else 
    {
        uint8_t value = 0xc9;
        /* Check primero del valor previo */
        if (i2c_read_byte(fd, MPU9250_XA_OFFSET_L, buf) == API_OK) printf("Valor previo : %#x\n", buf[0]);

        /* Escribo el valor que quiero */
        if (i2c_write_byte(fd, MPU9250_XA_OFFSET_L, value ) != API_OK)
        { 
            printf("FAIL");
        }
        else
        {
            if (i2c_read_byte(fd, MPU9250_XA_OFFSET_L, buf) != API_OK) printf("No se pudo leer");
            else printf("Valor a escrito : %#x. Valor leido : %#x\n", value, buf[0]);
        }
    }
    return 0;
}

int TEST_API_read_byte(void)
{
    printf("\n###Test : API_read_byte \n\n");
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        printf("Error abriendo fd");
    }
    else 
    {
        if (i2c_read_byte(fd, MPU9250_WHOIAM, buf) != API_OK) printf("FAIL");
        else printf("Returned value from %#x : {%#x}\n", MPU9250_WHOIAM, buf[0]);
    }
    return 0;
}

int TEST_API_read_bytes(void)
{
    printf("\n###Test : API_read_bytes \n\n");
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        printf("Error abriendo fd");
    }
    else 
    {   
        uint8_t len = 8;
        if (i2c_read_bytes(fd, MPU9250_ACCEL_XOUT_H, buf, len) != API_OK) 
        {
            printf("FAIL");
        }
        else 
        {
            printf("Returned values starting from %#x\n\n", MPU9250_ACCEL_XOUT_H);
            for (uint8_t i = 0; i < len; i++)
            {
                printf("Addr %#x -> Hex : %#x\n",MPU9250_ACCEL_XOUT_H + i, buf[i]);
            }
        }
    }
    return 0;
}



/*--------------------------------------------- API'S ---------------------------------------------------*/

retType i2c_read_byte(int fd, uint8_t reg, uint8_t *buffer)
{
    int ret = 0;
    buf[0] = reg;
    ret = read(fd, buffer, ONE_BYTE);
    if (ret < 0) return API_ERR;
    return API_OK;
}

retType i2c_read_bytes(int fd, uint8_t reg, uint8_t *buf, uint8_t len)
{
    int ret = 0;
    buf[0] = reg;
    ret = read(fd, buf, len);
    if (ret < 0) return API_ERR;
    return API_OK;
}

retType i2c_write_byte(int fd, uint8_t reg, uint8_t data)
{
    int ret = 0;
    uint8_t buffer[2];
    buffer[0] = reg;
    buffer[1] = data;
    ret = write(fd, buffer, ONE_BYTE);
    if (ret < 0) return API_ERR;
    return API_OK;
}
