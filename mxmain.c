static void mx_hbt_scan_instance(void)
{
    mx_uint32 ticks;
    mx_uint16 count;
    mx_uint16 elapse;

    ticks = mx_getTimeSinceBoot();
    mx_sys_ctx.delta += ticks - mx_sys_ctx.ticks;
    mx_sys_ctx.ticks  = ticks;

    count =
        (mx_sys_ctx.delta < 1500) ? 0 :
        (mx_sys_ctx.delta < 5500) ? 1 :
        (mx_sys_ctx.delta < 9500) ? 2 : 3;
    mx_sys_ctx.delta -= count * 1000;

    elapse =
        (mx_sys_ctx.delta <  800) ? 1200 :
        (mx_sys_ctx.delta < 1200) ? 1000 : 800;
    if (mx_sys_ctx.elapse != elapse)
    {
        mx_sys_ctx.elapse = elapse;
        mx_changeHeartbeat(elapse);
    }

    while (count != 0)
    {
        count--;
        mx_sys_ctx.count++;
        mx_sys_scan_instance();
        mx_mdm_scan_periodic();
        mx_soc_scan_anchor();
    }
}
