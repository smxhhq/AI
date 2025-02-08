
static void mx_socHandlerUdp(mx_uint16 session, const mx_socdata *data)
{
    mx_char * imei;
    mx_uint16 sum16;

    mx_uint8 *data1;
    mx_uint32 size1;
    mx_socdata package;

    mx_cfg.stats.amountOfDnload += data->size;

    mx_printf(MX_TRC_USER_EX, "mx_socHandlerUdp:");
    mx_puthex(MX_TRC_USER_EX, data->data, data->size);

    if (data->size < 14)
        return;

    if (data->data[0] != 'E' ||
        data->data[1] != 'L')
        return;

    if (data->data[2] != (mx_uint8)((data->size - 4) >> 8) ||
        data->data[3] != (mx_uint8)((data->size - 4) >> 0))
        return;

    sum16 = mx_sum16(0x0000, data->data + 6, data->size - 6);
    if (data->data[4] != (mx_uint8)(sum16 >> 8) ||
        data->data[5] != (mx_uint8)(sum16 >> 0))
        return;

    imei = mx_ctx.device.imei;
    if (data->data[ 6] != (imei[0] & 0x0F) ||
        data->data[ 7] != (mx_uint8)(((imei[ 1] & 0x0F) << 4) | (imei[ 2] & 0x0F)) ||
        data->data[ 8] != (mx_uint8)(((imei[ 3] & 0x0F) << 4) | (imei[ 4] & 0x0F)) ||
        data->data[ 9] != (mx_uint8)(((imei[ 5] & 0x0F) << 4) | (imei[ 6] & 0x0F)) ||
        data->data[10] != (mx_uint8)(((imei[ 7] & 0x0F) << 4) | (imei[ 8] & 0x0F)) ||
        data->data[11] != (mx_uint8)(((imei[ 9] & 0x0F) << 4) | (imei[10] & 0x0F)) ||
        data->data[12] != (mx_uint8)(((imei[11] & 0x0F) << 4) | (imei[12] & 0x0F)) ||
        data->data[13] != (mx_uint8)(((imei[13] & 0x0F) << 4) | (imei[14] & 0x0F)))
        return;

    data1 = data->data + 14;
    size1 = data->size - 14;
    package.addr = data->addr;
    while (size1 >= 7)
    {
        package.data = data1;
        package.size = 5 + ((package.data[3] << 8) | (package.data[4]));
        if ((package.data[0] == 0x67) &&
            (package.data[1] == 0x67) &&
            package.size <= size1)
        {
            mx_socHandler(&package);

            data1 += package.size;
            size1 -= package.size;
        }
        else
        {
            data1++;
            size1--;
        }
    }

    if (mx_ctx.server.itemUsed == 0)
        return;

    mx_purgePackage();
}