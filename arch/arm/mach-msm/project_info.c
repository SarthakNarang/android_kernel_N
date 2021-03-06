/*For OEM project information
*such as project name, hardware ID
*/
#include <linux/export.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/sys_soc.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/types.h>
#include <mach/msm_smem.h>
#include <linux/init.h>

#include <linux/project_info.h>

typedef		__u32		uint32;

struct project_info{
       char project_name[8];  //eg, 14049
       uint32  hw_version;  //PCB number, T0, EVT
       uint32  hw_version1;
       uint32  hw_version2;
       uint32  rf_v;
       uint32  rf_v1;
       uint32  rf_v2;
       uint32  modem;
       uint32  operator;
       uint32  ddr_manufacture_info;
       uint32  ddr_raw;
       uint32  ddr_column;
       uint32  ddr_reserve_info;
};

struct component_info{
	char *version;
	char *manufacture;
};

static struct component_info component_info_desc[COMPONENT_MAX];

static struct kobject *project_info_kobj;
static struct project_info * project_info_desc;

static struct kobject *component_info;

static ssize_t project_info_get(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t component_info_get(struct device *dev, struct device_attribute *attr, char *buf);

static DEVICE_ATTR(project_name, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(hw_id, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(hw_id1, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(hw_id2, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(rf_id_v, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(rf_id_v1, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(rf_id_v2, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(modem, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(operator_no, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(ddr_manufacture_info, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(ddr_raw, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(ddr_column, S_IRUGO, project_info_get, NULL);
static DEVICE_ATTR(ddr_reserve_info, S_IRUGO, project_info_get, NULL);

static ssize_t project_info_get(struct device *dev,
			    struct device_attribute *attr,
			    char *buf)
{
	if (attr == &dev_attr_project_name){
		return sprintf(buf, "%s\n", project_info_desc->project_name);
	}else if (attr == &dev_attr_hw_id){
		return sprintf(buf, "%d\n", project_info_desc->hw_version);
	}else if (attr == &dev_attr_hw_id1){
		return sprintf(buf, "%d\n", project_info_desc->hw_version1);
	}else if (attr == &dev_attr_hw_id2){
		return sprintf(buf, "%d\n", project_info_desc->hw_version2);
	}if (attr == &dev_attr_rf_id_v){
		return sprintf(buf, "%d\n", project_info_desc->rf_v);
	}if (attr == &dev_attr_rf_id_v1){
		return sprintf(buf, "%d\n", project_info_desc->rf_v1);
	}if (attr == &dev_attr_rf_id_v2){
		return sprintf(buf, "%d\n", project_info_desc->rf_v2);
	}if (attr == &dev_attr_modem){
		return sprintf(buf, "%d\n", project_info_desc->modem);
	}if (attr == &dev_attr_operator_no){
		return sprintf(buf, "%d\n", project_info_desc->operator);
	}if (attr == &dev_attr_ddr_manufacture_info){
		return sprintf(buf, "%d\n", project_info_desc->ddr_manufacture_info);
	}if (attr == &dev_attr_ddr_raw){
		return sprintf(buf, "%d\n", project_info_desc->ddr_raw);
	}if (attr == &dev_attr_ddr_column){
		return sprintf(buf, "%d\n", project_info_desc->ddr_column);
	}if (attr == &dev_attr_ddr_reserve_info){
		return sprintf(buf, "%d\n", project_info_desc->ddr_reserve_info);
    }
	return -EINVAL;

}

static struct attribute *project_info_sysfs_entries[] = {
	&dev_attr_project_name.attr,
	&dev_attr_hw_id.attr,
	&dev_attr_hw_id1.attr,
	&dev_attr_hw_id2.attr,
	&dev_attr_rf_id_v.attr,
	&dev_attr_rf_id_v1.attr,
	&dev_attr_rf_id_v2.attr,
	&dev_attr_modem.attr,
	&dev_attr_operator_no.attr,
	&dev_attr_ddr_manufacture_info.attr,
	&dev_attr_ddr_raw.attr,
	&dev_attr_ddr_column.attr,
	&dev_attr_ddr_reserve_info.attr,
	NULL,
};

static struct attribute_group project_info_attr_group = {
	.attrs	= project_info_sysfs_entries,
};

static DEVICE_ATTR(ddr, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(emmc, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(f_camera, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(r_camera, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(tp, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(lcd, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(wcn, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(l_sensor, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(g_sensor, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(m_sensor, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(gyro, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(backlight, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(mainboard, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(fingerprints, S_IRUGO, component_info_get, NULL);
static DEVICE_ATTR(touch_key, S_IRUGO, component_info_get, NULL);

char *get_component_version( enum COMPONENT_TYPE type)
{
	if(type >= COMPONENT_MAX){
		pr_err("%s == type %d invalid\n",__func__,type);
		return "N/A";
	}
	return component_info_desc[type].version?:"N/A";
}

char *get_component_manufacture( enum COMPONENT_TYPE type)
{
	if(type >= COMPONENT_MAX){
		pr_err("%s == type %d invalid\n",__func__,type);
		return "N/A";
	}
	return component_info_desc[type].manufacture?:"N/A";

}

int push_component_info(enum COMPONENT_TYPE type, char *version, char * manufacture)
{
	if(type >= COMPONENT_MAX){
			pr_err("%s == type %d invalid\n",__func__,type);
			return -1;
	}
	component_info_desc[type].version = version;
	component_info_desc[type].manufacture = manufacture;

	return 0;
}
EXPORT_SYMBOL(push_component_info);

int reset_component_info(enum COMPONENT_TYPE type)
{
	if(type >= COMPONENT_MAX){
			pr_err("%s == type %d invalid\n",__func__,type);
			return -1;
	}
	component_info_desc[type].version = NULL;
	component_info_desc[type].manufacture = NULL;

	return 0;
}
EXPORT_SYMBOL(reset_component_info);


static struct attribute *component_info_sysfs_entries[] = {
	&dev_attr_ddr.attr,
	&dev_attr_emmc.attr,
	&dev_attr_f_camera.attr,
	&dev_attr_r_camera.attr,
	&dev_attr_tp.attr,
	&dev_attr_lcd.attr,
	&dev_attr_wcn.attr,
	&dev_attr_l_sensor.attr,
	&dev_attr_g_sensor.attr,
	&dev_attr_m_sensor.attr,
	&dev_attr_gyro.attr,
	&dev_attr_backlight.attr,
	&dev_attr_mainboard.attr,
	&dev_attr_fingerprints.attr,
    &dev_attr_touch_key.attr,
	NULL,
};

static struct attribute_group component_info_attr_group = {
	.attrs	= component_info_sysfs_entries,
};

static ssize_t component_info_get(struct device *dev,
			    struct device_attribute *attr,
			    char *buf)
{
	if (attr == &dev_attr_ddr)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(DDR), get_component_manufacture(DDR));
	if (attr == &dev_attr_emmc)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(EMMC), get_component_manufacture(EMMC));
	if (attr == &dev_attr_f_camera)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(F_CAMERA), get_component_manufacture(F_CAMERA));
	if (attr == &dev_attr_r_camera)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(R_CAMERA), get_component_manufacture(R_CAMERA));
	if (attr == &dev_attr_tp)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(TP), get_component_manufacture(TP));
	if (attr == &dev_attr_lcd)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(LCD), get_component_manufacture(LCD));
	if (attr == &dev_attr_wcn)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(WCN), get_component_manufacture(WCN));
	if (attr == &dev_attr_l_sensor)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(I_SENSOR), get_component_manufacture(I_SENSOR));
	if (attr == &dev_attr_g_sensor)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(G_SENSOR), get_component_manufacture(G_SENSOR));
	if (attr == &dev_attr_m_sensor)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(M_SENSOR), get_component_manufacture(M_SENSOR));
	if (attr == &dev_attr_gyro)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(GYRO), get_component_manufacture(GYRO));
	if (attr == &dev_attr_backlight)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(BACKLIGHT), get_component_manufacture(BACKLIGHT));
	if (attr == &dev_attr_mainboard)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(MAINBOARD), get_component_manufacture(MAINBOARD));
    if (attr == &dev_attr_fingerprints)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(FINGERPRINTS), get_component_manufacture(FINGERPRINTS));
    if (attr == &dev_attr_touch_key)
		return sprintf(buf, "VER:\t%s\nMANU:\t%s\n", get_component_version(TOUCH_KEY), get_component_manufacture(TOUCH_KEY));
    
	return -EINVAL;
}

static int __init project_info_init_sysfs(void)
{
	int error = 0;

	project_info_kobj = kobject_create_and_add("project_info", NULL);
	if (!project_info_kobj)
		return -ENOMEM;
	error = sysfs_create_group(project_info_kobj, &project_info_attr_group);
	if (error){
		pr_err("project_info_init_sysfs project_info_attr_group failure\n");
		return error;
	}

	component_info = kobject_create_and_add("component_info", project_info_kobj);
	pr_info("project_info_init_sysfs success\n");
	if (!component_info)
		return -ENOMEM;

	error = sysfs_create_group(component_info, &component_info_attr_group);
	if (error){
		pr_err("project_info_init_sysfs project_info_attr_group failure\n");
		return error;
	}
	return 0;
}

late_initcall(project_info_init_sysfs);

struct ddr_manufacture{
	int id;
	char name[20];
};
//ddr id and ddr name
static char ddr_version[32] = {0};
static char ddr_manufacture[20] = {0};

struct ddr_manufacture ddr_manufacture_list[]={
     {1,"Samsung "},
	 {2,"Qimonda "},
	 {3,"Elpida "},
	 {4,"Etpon "},
	 {5,"Nanya "},
	 {6,"Hynix "},
	 {7,"Mosel "},
	 {8,"Winbond "},
	 {9,"Esmt "},
	 {0,},
};

void get_ddr_manufacture_name(void){
	int i;
	int id = project_info_desc->ddr_manufacture_info;
	for(i = 0; i < (sizeof(ddr_manufacture_list)/sizeof(ddr_manufacture_list[0])); i++)
	{
		if(ddr_manufacture_list[i].id == id)
		{
			sprintf(ddr_manufacture, "%s", ddr_manufacture_list[i].name);
			break;
		}
	}
}

#define PROJECT_14001_NAME "14001"
#define PROJECT_15055_NAME "15055"

#define PROJCET_NAME_LEN 17

static bool is_14001_projcet;
static bool is_15055_projcet;
static char project_name[PROJCET_NAME_LEN];

const char *get_prj_name(void)
{
    return project_name;
}

bool is_14001_project(void)
{
    return is_14001_projcet;
}

bool is_15055_project(void)
{
    return is_15055_projcet;
}

static int __init parse_project_name(char *str)
{
    if(str == NULL){
        return -1;
    }

    snprintf(project_name, PROJCET_NAME_LEN, "%s", str);

    if(!strncmp(project_name, PROJECT_15055_NAME, sizeof(PROJECT_15055_NAME))){
        is_15055_projcet = true;
        is_14001_projcet = false;
    }else if(!strncmp(project_name, PROJECT_14001_NAME, sizeof(PROJECT_14001_NAME))){
        is_14001_projcet = true;
        is_15055_projcet = false;
    }else{ // default it is 14001 project.
        is_14001_projcet = true;
        is_15055_projcet = false;
    }

    return 0;
}

early_param("prj_name", parse_project_name);

/*
sbl1 : platforminfo.h

enum{
	HW_VERSION__UNKNOWN,
	HW_VERSION__10,		//729mV
	HW_VERSION__11,		//900 mV
	HW_VERSION__12,		//1200 mV   ============3 ======== 
	HW_VERSION__13,		//1484 mV

	HW_VERSION__20 = 20,		
	HW_VERSION__21 = 21,		
	HW_VERSION__22,		
	HW_VERSION__23,	
	
	HW_VERSION__30 = 30,	//N3 EVB	
	HW_VERSION__31 = 31,	//N3 EVT		
	HW_VERSION__32,			//N3 DVT	
	HW_VERSION__33,			//N3 PVT	
	HW_VERSION__34, 		//N3 Mp	
	HW_VERSION__35, 		//N3 ??	

	HW_VERSION__40 = 40,	//oversea N3 EVB	
	HW_VERSION__41 = 41,	//oversea N3 EVT		
	HW_VERSION__42,			//oversea N3 DVT	
	HW_VERSION__43,			//oversea N3 PVT	
	HW_VERSION__44, 		//oversea N3 Mp	
	HW_VERSION__45, 		//oversea N3 ??		
};

enum{
	RF_VERSION__UNKNOWN,
	RF_VERSION__11,		
	RF_VERSION__12,		
	RF_VERSION__13,
	RF_VERSION__21,		
	RF_VERSION__22,		
	RF_VERSION__23,
	RF_VERSION__31,		
	RF_VERSION__32,		
	RF_VERSION__33,
	RF_VERSION__44 = 19,
	RF_VERSION__66 = 30,//add for 14001 TDD-LTE+FDD-LTE+TDS+W+G
	RF_VERSION__67,
	RF_VERSION__76,
	RF_VERSION__77,
	RF_VERSION__87,
	RF_VERSION__88,
	RF_VERSION__89,
	RF_VERSION__98,  ============37 ======== 
	RF_VERSION__99,
	RF_VERSION__90_CHINA_MOBILE= 90,
	RF_VERSION__91_UNICOM,
	RF_VERSION__92_CHINA_RESERVED1,
	RF_VERSION__93_CHINA_RESERVED2,
	RF_VERSION__94_CHINA_RESERVED3,
	RF_VERSION__95_EUROPE,
	RF_VERSION__96_AMERICA,
	RF_VERSION__97_TAIWAN,
	RF_VERSION__98_INDONESIA,	
	RF_VERSION__99_OVERSEA_RESERVED1,	
};
*/
static char mainboard_version[4] = {0};
static char mainboard_manufacture[8] = {'O', 'N', 'E', 'P', 'L', 'U', 'S','\0'};

int __init init_project_info(void)
{
	static bool project_info_init_done;

	if (project_info_init_done)
		return 0;

	project_info_desc = smem_find(SMEM_PROJECT_INFO,
				sizeof(struct project_info)/*,
				0,
				SMEM_ANY_HOST_FLAG*/);

	if (IS_ERR_OR_NULL(project_info_desc)){
		pr_err("%s: get project_info failure\n",__func__);
		return -1;
	}else{
		pr_info("%s: project_name: %s; hw_version: %d; hw_version1: %d; hw_version2: %d; rf_v: %d; rf_v1: %d; rf_v2: %d\n",
						__func__, project_info_desc->project_name, project_info_desc->hw_version,
						project_info_desc->hw_version1, project_info_desc->hw_version2,
						project_info_desc->rf_v, project_info_desc->rf_v1, project_info_desc->rf_v2);

		pr_info("%s: ddr_column: %d; ddr_manufacture_info: %d; ddr_raw: %d; ddr_reserve_info: %d\n",
						__func__, project_info_desc->ddr_column, project_info_desc->ddr_manufacture_info,
								project_info_desc->ddr_raw, project_info_desc->ddr_reserve_info);
    }

	snprintf(mainboard_version, sizeof(mainboard_version), "%d", project_info_desc->hw_version);
	push_component_info(MAINBOARD, mainboard_version, mainboard_manufacture);

	//add ddr row, column information and manufacture name information
	get_ddr_manufacture_name();
	snprintf(ddr_version, sizeof(ddr_version), "row:%d, column:%d", project_info_desc->ddr_raw, project_info_desc->ddr_column);
	push_component_info(DDR, ddr_version, ddr_manufacture);

	project_info_init_done = true;

	return 0;
}
subsys_initcall(init_project_info);
