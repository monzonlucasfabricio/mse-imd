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
#define MPU9250_GYRO_XOUT_H 0x43
#define MPU9250_GYRO_XOUT_L 0x44
#define MPU9250_GYRO_YOUT_H 0x45
#define MPU9250_GYRO_YOUT_L 0x46
#define MPU9250_GYRO_ZOUT_H 0x47
#define MPU9250_GYRO_ZOUT_L 0x48


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

/* Declaracion de MPU API's */
retType mpu9250_get_temp(uint16_t *temp);
retType mpu9250_get_accel(uint16_t *x, uint16_t *y, uint16_t *z);
retType mpu9250_get_gyro(uint16_t *x, uint16_t *y, uint16_t *z);

int main(void)
{
    uint8_t opcion;
    uint8_t reg;
    uint8_t len;
    uint8_t data;
    /* Menu */
    printf("\n ---- MENU ----\n");
    printf("1. Leer un byte\n");
    printf("2. Leer mas de un byte (max = 255)\n");
    printf("3. Escribir un byte\n");
    printf("4. Test de lectura\n");
    printf("5. Test de escritura\n");
    printf("6. Acelerometro\n");
    printf("7. Giroscopio\n");


    printf("\nopcion: ");
    scanf("%d", &opcion);

    switch(opcion)
    {
        case 1:
        {
            printf("reg: ");
            scanf("%i",&reg);
            int fd = open(DEVICE, O_RDONLY);
            if (fd < 0)
            {
                printf("No se pudo abrir el file descriptor\n");
                return 0;
            }
            i2c_read_byte(fd, reg, buf);
            printf("res: %#x\n",buf[0]);
        }
        break;

        case 2:
        {
            printf("reg: ");
            scanf("%i",&reg);
            printf("len: ");
            scanf("%i",&len);
            int fd = open(DEVICE, O_RDONLY);
            if (fd < 0)
            {
                printf("No se pudo abrir el file descriptor\n");
                return 0;
            }
            i2c_read_bytes(fd, reg, buf, len);
            printf("res:\n");
            for(uint8_t i = 0; i < len; i++)
            {
                printf("%d: %#x\n",i + 1, buf[i]);
            }
        }
        break;

        case 3:
        {
            printf("reg: ");
            scanf("%i",&reg);
            printf("data: ");
            scanf("%i",&data);
            int fd = open(DEVICE, O_RDWR);
            if (fd < 0)
            {
                printf("No se pudo abrir el file descriptor\n");
                return 0;
            }
            if (i2c_write_byte(fd, reg, data) != API_OK)
            {
                printf("Error al escribir");
            }
        }
        break;

        case 4:
        {
            TEST_API_read_bytes();
        }
        break;

        case 5:
        {
            TEST_API_write_byte();
        }
        break;

        case 6:
        {
            while(1)
            {
                uint16_t x, y, z;
                mpu9250_get_accel(&x, &y, &z);
                printf("x: %i  y: %i  z: %i\n",x,y,z);
                usleep(200000);
            }
        }
        break;

        case 7:
        {
            while(1)
            {
                uint16_t x, y, z;
                mpu9250_get_gyro(&x, &y, &z);
                printf("x: %i  y: %i  z: %i\n",x,y,z);
                usleep(200000);
            }
        }
        break;
    }
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
        return 0;
    }
    else 
    {
        uint8_t value = 0x02;
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
    close(fd);
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
    close(fd);
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
    close(fd);
    return 0;
}



/*--------------------------------------------- I2C API'S ---------------------------------------------------*/

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


/*--------------------------------------------- MPU API'S ---------------------------------------------------*/

retType mpu9250_get_temp(uint16_t *temp)
{
    uint8_t len = 2;
    uint16_t aux_temp;
    int fd = open(DEVICE, O_RDONLY);
    if (i2c_read_bytes(fd, MPU9250_TEMP_H, buf, len) != API_OK)
    {
        printf("\n No se pudo leer el registro de temperatura\n");
        return API_ERR;
    }
    else
    {
        aux_temp = (uint16_t)buf[0] << 8 | buf[1];
        *temp = aux_temp;
    }
    close(fd);
    return API_OK;
}


retType mpu9250_get_accel(uint16_t *x, uint16_t *y, uint16_t *z)
{
    static uint64_t time = 0;
    uint8_t len = 6;
    uint16_t aux_temp;
    int fd = open(DEVICE, O_RDONLY);
    if (i2c_read_bytes(fd, MPU9250_ACCEL_XOUT_H, buf, len) != API_OK)
    {
        printf("\n No se pudo leer el registro\n");
        close(fd);
        return API_ERR;
    }
    else
    {
        *x = (uint16_t)buf[0] << 8 | buf[1];
        *y = (uint16_t)buf[2] << 8 | buf[3];
        *z = (uint16_t)buf[4] << 8 | buf[5];

        FILE *f;
        f = fopen("datos.csv","w");
        if (f == NULL)
        {
            printf("No se pudo abrir el archivo.\n");
            return API_ERR;
        }
        
        fprintf(f, "T,X,Y,Z\n");
        fprintf(f, "%i,%u,%u,%u\n",time++,*x,*y,*z);

        fclose(f);
    }
    close(fd);
    return API_OK;
}

retType mpu9250_get_gyro(uint16_t *x, uint16_t *y, uint16_t *z)
{
    uint8_t len = 6;
    uint16_t aux_temp;
    int fd = open(DEVICE, O_RDONLY);
    if (i2c_read_bytes(fd, MPU9250_GYRO_XOUT_H, buf, len) != API_OK)
    {
        printf("\n No se pudo leer el registro\n");
        close(fd);
        return API_ERR;
    }
    else
    {
        *x = (uint16_t)buf[0] << 8 | buf[1];
        *y = (uint16_t)buf[2] << 8 | buf[3];
        *z = (uint16_t)buf[4] << 8 | buf[5];
    }
    close(fd);
    return API_OK;
}

