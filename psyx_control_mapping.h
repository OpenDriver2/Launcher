#ifndef _Launcher_psyx_control_mapping_h_
#define _Launcher_psyx_control_mapping_h_

#define CONTROLLER_MAP_FLAG_AXIS		0x4000
#define CONTROLLER_MAP_FLAG_INVERSE		0x8000

typedef struct
{
	int id;

	int kc_square, kc_circle, kc_triangle, kc_cross;

	int kc_l1, kc_l2, kc_l3;
	int kc_r1, kc_r2, kc_r3;

	int kc_start, kc_select;

	int kc_dpad_left, kc_dpad_right, kc_dpad_up, kc_dpad_down;
} PsyXKeyboardMapping;

typedef struct
{
	int id;

	// you can bind axis by adding CONTROLLER_MAP_AXIS_FLAG
	int gc_square, gc_circle, gc_triangle, gc_cross;

	int gc_l1, gc_l2, gc_l3;
	int gc_r1, gc_r2, gc_r3;

	int gc_start, gc_select;

	int gc_dpad_left, gc_dpad_right, gc_dpad_up, gc_dpad_down;

	int gc_axis_left_x, gc_axis_left_y;
	int gc_axis_right_x, gc_axis_right_y;
} PsyXControllerMapping;

typedef struct ini_t ini_t;

void DefaultMappings(PsyXKeyboardMapping& mapping);
void DefaultMappings(PsyXControllerMapping& mapping);

void ParseControllerMappings(ini_t* config, char* section, PsyXControllerMapping& outMapping);
void ParseKeyboardMappings(ini_t* config, char* section, PsyXKeyboardMapping& outMapping);

#endif
