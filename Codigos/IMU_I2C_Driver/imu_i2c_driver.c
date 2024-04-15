#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/of.h>

#define MAX_VALUES 255

static int mpu9250_read_reg(struct i2c_client *client, u8 reg, u8 *data, size_t len);
static int mpu9250_write_reg(struct i2c_client *client, u8 reg, u8 data);


#define MPU9250_READ_BYTES(client, reg, data, len)   mpu9250_read_reg(client, (reg), (data), (len))
#define MPU9250_WRITE_BYTES(client, reg, data)  mpu9250_write_reg(client, (reg), (data))

/* Private device structure */
struct mse_dev
{
	struct i2c_client *client;
	struct miscdevice mse_miscdevice;
	char name[11]; /* msedrvXX */
};


/*
 * Definicion de los ID correspondientes al Device Tree. Estos deben ser informados al
 * kernel mediante la macro MODULE_DEVICE_TABLE
 *
 * NOTA: Esta seccion requiere que CONFIG_OF=y en el kernel
 */

static const struct of_device_id mse_dt_ids[] =
{
    { .compatible = "mpu_i2c_9250", },
    { /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, mse_dt_ids);


/* User is reading data from /dev/msedrvXX */
static ssize_t imu_i2c_read(struct file *file, char __user *userbuf, size_t count, loff_t *ppos)
{
	struct mse_dev *mse;
    int ret = 0;
    uint8_t data[MAX_VALUES];

    /* Control de errores en los parametros de entrada */
    if (count > MAX_VALUES || count <= 0) return -EINVAL;
    if (userbuf == NULL) return -EINVAL;

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

    /* Operacion de lectura */
    ret = MPU9250_READ_BYTES(mse->client, *userbuf, data, count);
    if (ret < 0)
    {
        pr_err("Error al leer registros: %d\n",ret);
        return -EIO;
    }

    /* Copio en el buffer para devolverlos */
    memcpy(userbuf, data, count);

    return 0;
}

static ssize_t imu_i2c_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    struct mse_dev *mse;
    int ret;

    /* Control de errores en los parametros de entrada */
    if (len <= 0 || len > 1) return -EINVAL;
    if (buffer == NULL) return -EINVAL;

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

    /* Operacion de escritura */
    ret = MPU9250_WRITE_BYTES(mse->client, buffer[0], buffer[1]);
    if (ret < 0)
    {
        pr_err("Error al escribir registros");
        return -EIO;
    }

    return 0;
}

static long imu_i2c_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct mse_dev *mse;

    mse = container_of(file->private_data, struct mse_dev, mse_miscdevice);

    pr_info("my_dev_ioctl() fue invocada. cmd = %d, arg = %ld\n", cmd, arg);

    return 0;
}

/* declaracion de una estructura del tipo file_operations */

static const struct file_operations mse_fops =
{
    .owner = THIS_MODULE,
    .read = imu_i2c_read,
    .write = imu_i2c_write,
    .unlocked_ioctl = imu_i2c_ioctl,
};

/*--------------------------------------------------------------------------------*/
static int imu_i2c_probe(struct i2c_client *client)
{
    struct mse_dev *mse;
    static int counter = 0;
    int ret_val;

    /* Allocate new private structure */
    mse = devm_kzalloc(&client->dev, sizeof(struct mse_dev), GFP_KERNEL);

    /* Store pointer to the device-structure in bus device context */
    i2c_set_clientdata(client,mse);

	/* Store pointer to I2C client device in the private structure */
	mse->client = client;

    /* Initialize the misc device, mse is incremented after each probe call */
    sprintf(mse->name, "mse%02d", counter++);

    mse->mse_miscdevice.name = mse->name;
    mse->mse_miscdevice.minor = MISC_DYNAMIC_MINOR;
    mse->mse_miscdevice.fops = &mse_fops;

    /* Register misc device */
    ret_val = misc_register(&mse->mse_miscdevice);

    if (ret_val != 0)
    {
        pr_err("No se pudo registrar el dispositivo %s\n", mse->mse_miscdevice.name);
        return ret_val;
    }

    pr_info("Dispositivo %s: minor asignado: %i\n", mse->mse_miscdevice.name, mse->mse_miscdevice.minor);

    return 0;
}

static void imu_i2c_remove(struct i2c_client * client)
{
    struct mse_dev * mse;

    /* Get device structure from bus device context */
    mse = i2c_get_clientdata(client);

    /* Deregister misc device */
    misc_deregister(&mse->mse_miscdevice);
}

/*--------------------------------------------------------------------------------*/

static struct i2c_driver mse_driver =
{
    .probe= imu_i2c_probe,
    .remove= imu_i2c_remove,
    .driver =
    {
        .name = "i2c_imu_9250/6500",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(mse_dt_ids),
    },
};

/*----------------------------------------------------------------------*/


/*----------------- FUNCIONES EXTRAS DEL DRIVER------------------------ */

static int mpu9250_read_reg(struct i2c_client *client, u8 reg, u8 *data, size_t len)
{
    struct i2c_msg msg;
    u8 buf[1] = {reg};

    msg.addr = client->addr;
    msg.flags = 0;
    msg.buf = buf;
    msg.len = sizeof(buf);

    if (i2c_transfer(client->adapter, &msg, 1) != 1)
        return -EIO;

    msg.flags = I2C_M_RD;
    msg.buf = data;
    msg.len = len;

    if (i2c_transfer(client->adapter, &msg, 1) != 1)
        return -EIO;

    return 0;
}

static int mpu9250_write_reg(struct i2c_client *client, u8 reg, u8 data)
{
    struct i2c_msg msg;
    u8 buf[2] = {reg, data};

    msg.addr = client->addr;
    msg.flags = 0;
    msg.buf = buf;
    msg.len = sizeof(buf);

    if (i2c_transfer(client->adapter, &msg, 1) != 1)
        return -EIO;

    return 0;
}



/**********************************************************************
 * Esta seccion define cuales funciones seran las ejecutadas al cargar o
 * remover el modulo respectivamente. Es hecho implicitamente,
 * termina declarando init() exit()
 **********************************************************************/
module_i2c_driver(mse_driver);

/**********************************************************************
 * Seccion sobre Informacion del modulo
 **********************************************************************/
MODULE_AUTHOR("Lucas Monzon Languasco <monzonlucasfabricio@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Es un driver para IMU (Unidad de Medicion Inercial) MPU 9250 / 6500");
MODULE_INFO(mse_imd,"MPU 9250/6500");
