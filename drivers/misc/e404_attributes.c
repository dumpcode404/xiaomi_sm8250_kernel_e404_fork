// E404 kernel helper by Project 113 (kvsnr113)

#include <linux/e404_attributes.h>

#ifdef CONFIG_E404_EFFCPU_DEFAULT
int early_effcpu = 1;
#else
int early_effcpu = 0;
#endif
#ifdef CONFIG_E404_MIUI_DTBO_DEFAULT
int early_rom_type = 2;
#else
int early_rom_type = 1;
#endif
#ifdef CONFIG_E404_MIUI_DTBO_DEFAULT
int early_dtbo_type = 2;
#else
int early_dtbo_type = 1;
#endif
#ifdef CONFIG_E404_ALIOTH_5K_BATT_DEFAULT
int early_batt_profile = 2;
#else
int early_batt_profile = 1;
#endif

struct e404_attributes e404_data = {
    .effcpu                     = 0,
    .rom_type                   = 1,
    .dtbo_type                  = 0,
    .batt_profile               = 1,
    .kgsl_skip_zeroing          = 0,
    .file_sync                  = 1,
#ifdef CONFIG_BOARD_PIPA
    .panel_oem_width_pipa       = 1662,
    .panel_oem_height_pipa      = 2660,
    .panel_width_pipa           = 166,
    .panel_height_pipa          = 266,
#else
    .panel_width                = 70,
    .panel_height               = 155,
    .panel_oem_width            = 700,
    .panel_oem_height           = 1550,
#endif
};

static struct kobject *e404_kobj;

static int __init parse_e404_args(char *str)
{
    char *arg;

    while ((arg = strsep(&str, " ,")) != NULL) {
        if (!*arg) continue;

        pr_alert("E404: Parsing flag: %s\n", arg);

        if (strcmp(arg, "dtb_effcpu") == 0)
            early_effcpu = 1;
        else if (strcmp(arg, "dtb_def") == 0)
            early_effcpu = 0;
        else if (strcmp(arg, "rom_oem") == 0)
            early_rom_type = 2;
        else if (strcmp(arg, "rom_aosp") == 0)
            early_rom_type = 1;
#ifdef CONFIG_BOARD_PIPA
        else if (strcmp(arg, "dtbo_def_pipa") == 0)
            early_dtbo_type = 3;
        else if (strcmp(arg, "dtbo_oem_pipa") == 0)
            early_dtbo_type = 4;
#else
        else if (strcmp(arg, "dtbo_def") == 0)
            early_dtbo_type = 1;
        else if (strcmp(arg, "dtbo_oem") == 0)
            early_dtbo_type = 2;
#endif
        else if (strcmp(arg, "batt_def") == 0)
            early_batt_profile = 1;
        else if (strcmp(arg, "batt_5k") == 0)
            early_batt_profile = 2;
        else
            pr_alert("E404: Unknown flag: %s\n", arg);
    }

    return 0;
}
early_param("e404_args", parse_e404_args);

#define E404_ATTR_RO(name) \
static ssize_t name##_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) { \
    return sprintf(buf, "%d\n", e404_data.name); \
} \
static struct kobj_attribute name##_attr = __ATTR(name, 0444, name##_show, NULL);

#define E404_ATTR_RW(name) \
static ssize_t name##_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) { \
    return sprintf(buf, "%d\n", e404_data.name); \
} \
static ssize_t name##_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) { \
    int ret, val, old_val; \
    ret = kstrtoint(buf, 10, &val); \
    if (ret) return ret; \
    old_val = e404_data.name; \
    e404_data.name = val; \
    pr_alert("E404: %s changed from %d to %d\n", #name, old_val, val); \
    sysfs_notify(e404_kobj, NULL, #name); \
    return count; \
} \
static struct kobj_attribute name##_attr = __ATTR(name, 0664, name##_show, name##_store);

E404_ATTR_RO(effcpu);
E404_ATTR_RO(rom_type);
E404_ATTR_RO(dtbo_type);
E404_ATTR_RO(batt_profile);
#ifdef CONFIG_BOARD_PIPA
E404_ATTR_RO(panel_width_pipa);
E404_ATTR_RO(panel_height_pipa);
E404_ATTR_RO(panel_oem_width_pipa);
E404_ATTR_RO(panel_oem_height_pipa);
#else
E404_ATTR_RO(panel_width);
E404_ATTR_RO(panel_height);
E404_ATTR_RO(panel_oem_width);
E404_ATTR_RO(panel_oem_height);
#endif

E404_ATTR_RW(kgsl_skip_zeroing);
E404_ATTR_RW(file_sync);

static struct attribute *e404_attrs[] = {
    &kgsl_skip_zeroing_attr.attr,
    &file_sync_attr.attr,
    NULL,
};

static struct attribute_group e404_group = {
    .attrs = e404_attrs,
};

static struct attribute *e404_prop_attrs[] = {
    &effcpu_attr.attr,
    &rom_type_attr.attr,
    &dtbo_type_attr.attr,
    &batt_profile_attr.attr,
#ifdef CONFIG_BOARD_PIPA
    &panel_width_pipa_attr.attr,
    &panel_height_pipa_attr.attr,
    &panel_oem_width_pipa_attr.attr,
    &panel_oem_height_pipa_attr.attr,
#else
    &panel_width_attr.attr,
    &panel_height_attr.attr,
    &panel_oem_width_attr.attr,
    &panel_oem_height_attr.attr,
#endif
    NULL,
};

static struct attribute_group e404_prop_group = {
    .name  = "prop",
    .attrs = e404_prop_attrs,
};

static void e404_parse_attributes(void) {
    e404_data.effcpu      = early_effcpu;
    e404_data.rom_type    = early_rom_type;
    e404_data.dtbo_type   = early_dtbo_type;
    e404_data.batt_profile = early_batt_profile;
}

static int __init e404_init(void) {
    int ret;

    e404_parse_attributes();

    e404_kobj = kobject_create_and_add("e404", kernel_kobj);
    if (!e404_kobj)
        return -ENOMEM;

    ret = sysfs_create_group(e404_kobj, &e404_group);
    if (ret)
        goto fail_kobj;

    ret = sysfs_create_group(e404_kobj, &e404_prop_group);
    if (ret)
        goto fail_group;

    pr_alert("E404: Helper Init !\n");
    return 0;

fail_group:
    sysfs_remove_group(e404_kobj, &e404_group);
fail_kobj:
    kobject_put(e404_kobj);
    return ret;
}

static void __exit e404_exit(void) {
    sysfs_remove_group(e404_kobj, &e404_prop_group);
    sysfs_remove_group(e404_kobj, &e404_group);
    kobject_put(e404_kobj);
    pr_alert("E404: Helper Exit !\n");
}

core_initcall(e404_init);
module_exit(e404_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kvsnr113");
MODULE_DESCRIPTION("E404 kernel helper for features & stuff");
MODULE_VERSION("1.6");