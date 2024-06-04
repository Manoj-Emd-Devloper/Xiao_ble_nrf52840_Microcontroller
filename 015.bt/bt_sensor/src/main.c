#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/bluetooth/bluetooth.h>

LOG_MODULE_REGISTER(ble_sensor);

volatile bool ble_ready = false;
struct sensor_value pre, temp;
uint8_t temperature , pressure;
const struct device *const dev = DEVICE_DT_GET_ONE(st_lps22hb_press);

static void process_sample(const struct device *dev)
{

        if (sensor_sample_fetch(dev) < 0)
        {
                LOG_ERR("Sensor sample update error\n");
                return;
        }

        if (sensor_channel_get(dev, SENSOR_CHAN_PRESS, &pre) < 0)
        {
                LOG_ERR("Cannot read LPS22HB pressure channel\n");
                return;
        }

        if (sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0)
        {
                LOG_ERR("Cannot read LPS22HB temperature channel\n");
                return;
        }

        /* display pressure */
        pressure = (uint8_t)sensor_value_to_double(&pre);
        LOG_INF("Pressure:%u kPa\n", pressure);

        /* display temperature */
        temperature = (uint8_t)sensor_value_to_double(&temp);
        LOG_INF("Temperature:%u C\n", temperature);

        LOG_INF("=========================\n\r");
}

// for advertising
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_ESS_VAL))};

ssize_t my_read_temperature_fuction(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr, void *buf,
                                    uint16_t len, uint16_t offset);
ssize_t my_read_pressure_fuction(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr, void *buf,
                                    uint16_t len, uint16_t offset);
BT_GATT_SERVICE_DEFINE(ess_srv,
                       BT_GATT_PRIMARY_SERVICE(BT_UUID_ESS),
                       BT_GATT_CHARACTERISTIC(BT_UUID_TEMPERATURE, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, my_read_temperature_fuction, NULL, NULL),
                       BT_GATT_CHARACTERISTIC(BT_UUID_PRESSURE, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, my_read_pressure_fuction, NULL, NULL));

ssize_t my_read_temperature_fuction(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr, void *buf,
                                    uint16_t len, uint16_t offset)
{
        return bt_gatt_attr_read(conn, attr, buf, len, offset, &temperature,
                                 sizeof(temperature));
}
ssize_t my_read_pressure_fuction(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr, void *buf,
                                 uint16_t len, uint16_t offset)
{
        return bt_gatt_attr_read(conn, attr, buf, len, offset, &pressure,
                                 sizeof(pressure));
}
void bt_ready(int err)
{
        if (err)
        {
                LOG_ERR("bt enable return %d", err);
        }
        LOG_INF("bt_ready!");
        ble_ready = true;
}

// BT INITILLIZATION
int init_ble(void)
{
        LOG_INF("init BLE");
        int err;
        err = bt_enable(bt_ready);
        if (err)
        {
                LOG_ERR("bt_enable failed (err %d)", err);
                return err;
        }
        return 0;
}

void main(void)
{
        if (!device_is_ready(dev))
        {
                LOG_INF("Device %s is not ready\n", dev->name);
                return;
        }

        init_ble();
        while (!ble_ready)
        {
                LOG_INF("BLE stack not ready yet");
                k_msleep(100);
        }

        LOG_INF("BLE stack ready");

        int err;
        err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
        if (err)
        {
                LOG_ERR("Adertising failed to start (err %d)", err);
                return;
        }

        while (true)
        {
                process_sample(dev);
                k_sleep(K_MSEC(2000));
        }
}
