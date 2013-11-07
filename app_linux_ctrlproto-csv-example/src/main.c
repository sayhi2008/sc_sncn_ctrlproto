#include <ctrlproto_m.h>
#include <ecrt.h>
#include "ethercat_setup.h"
#include <stdio.h>
#include <stdbool.h>
#include "profile.h"
#include "drive_function.h"
#include <motor_define.h>
#include <sys/time.h>
#include <time.h>


int main()
{
	int flag = 0;

	int final_target_velocity = -1000;			//rpm
	int initial_velocity = 0;					//rpm
	int acceleration= 1000;						//rpm/s
	int deceleration = 1000;					//rpm/s
	int steps = 0;
	int i = 1;
	int target_velocity = 0;
	int actual_velocity = 0;

	int slave_number = 0;
	printf(" gear %d", slv_handles[slave_number].motor_config_param.s_gear_ratio.gear_ratio);
	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	init_node(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	i = 0;
	steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration, MAX_PROFILE_VELOCITY(1));

	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_velocity = velocity_profile_generate(i);
				set_velocity_rpm(target_velocity, slave_number, slv_handles);
				actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				printf("velocity %d \n",actual_velocity);
				i = i+1;
			}
			if(i>=steps && flag == 0)
			{
				initial_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				final_target_velocity = 2000; //rpm
				steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration, MAX_PROFILE_VELOCITY(1));
				i = 1;
				flag = 1;
			}
			if(i>=steps && flag == 1)
			{
				initial_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				final_target_velocity = -1000;	//rpm
				steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration, MAX_PROFILE_VELOCITY(1));
				i = 1;
				flag = 2;
			}
			if(i >= steps && flag == 2)
			{
				break;
			}
		}
	}

	quick_stop_velocity(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	renable_ctrl_quick_stop(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	i = 0;
	flag = 0;
	final_target_velocity = 1000; //rpm
	initial_velocity = get_velocity_actual_rpm(slave_number, slv_handles); //rpm
	steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration, MAX_PROFILE_VELOCITY(1));

	while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)//Check if we are up
		{
			if(i<steps)
			{
				target_velocity = velocity_profile_generate(i);
				set_velocity_rpm(target_velocity, slave_number, slv_handles);
				actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				printf("velocity %d \n",actual_velocity);
				i = i+1;
			}
			if(i>=steps && flag == 0)
			{
				initial_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
				final_target_velocity = 0; //rpm
				steps = init_velocity_profile(final_target_velocity, initial_velocity, acceleration, deceleration, MAX_PROFILE_VELOCITY(1));
				i = 1;
				flag = 1;
			}
			if(i >= steps && flag == 1)
			{
				break;
			}
		}
	}



	shutdown_operation(CSV, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);
//*/
	return 0;
}


