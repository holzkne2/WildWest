#include "main.h"

#include <math.h>

static void game_output_sound(const game_sound_output_buffer& sound_buffer_p, int32 tone_hz)
{
	static real32 t_sine = 0;
	const static int16 tone_volume = 16000;
	int32 wave_period = sound_buffer_p.samples_per_second / tone_hz;
	
	int16 *sample_out = sound_buffer_p.samples;
	for (int32 sample_index = 0; sample_index < sound_buffer_p.sample_count; ++sample_index)
	{
		real32 sine_value = sinf(t_sine);
		int16 sample_value = (int16)(sine_value * tone_volume);
		*sample_out++ = sample_value;
		*sample_out++ = sample_value;

		t_sine += 2.0f * PI32 * 1.0f / (real32)wave_period;
	}
}

static void render_weird_grandent(const game_offscreen_buffer& buffer_p, int32 x_offset, int32 y_offset)
{
	uint8 *row = (uint8 *)buffer_p.memory;
	for (int32 y = 0; y < buffer_p.height; ++y)
	{
		uint32 *pixel = (uint32 *)row;
		for (int32 x = 0; x < buffer_p.width; ++x)
		{
			/*
			Pixel in memory: BB GG RR xx
			Little Endian Architecture 0x xxRRGGBB
			*/

			uint8 b = (uint8)(x + x_offset);
			uint8 g = (uint8)(y + y_offset);

			*pixel++ = ((g << 8) | b);
		}
		row += buffer_p.pitch;
	}
}

void game_update_and_render(game_memory& memory_p, const game_input& input_p,
	game_offscreen_buffer& buffer_p, game_sound_output_buffer& sound_buffer_p)
{
	game_state& game_state_p = (game_state &)memory_p.permanent_storage_p;
	if (!memory_p.is_initialized) {
#if defined(_DEBUG)
		const char * file_name = __FILE__;
		
		read_file_DEBUG file = platform_read_entire_file_DEBUG(file_name);
		if (file.memory != nullptr) {
			platform_write_entire_file_DEBUG("../../temp/file_writtings_test.txt",
				file.size, file.memory);
			platform_free_file_memory_DEBUG(file);
		}
#endif // #if defined(_DEBUG)

		game_state_p.tone_hz  = 256;
		game_state_p.x_offset = 0;
		game_state_p.y_offset = 0;

		memory_p.is_initialized = true;
	}

	game_state_p.tone_hz = 256;

	const game_controller_input& input_0 = input_p.controllers[0]; //keyboard
	const game_controller_input& input_1 = input_p.controllers[1]; //controller
	if (input_1.is_connected == true)
	{
		game_state_p.tone_hz += (int32)(128.0f * input_1.left_stick.average_y);
		game_state_p.x_offset += (int32)(4.8f * input_1.left_stick.average_x);
	}
	{
		game_state_p.tone_hz += (int32)(128.0f * (int32)input_0.down.ended_down);
		game_state_p.tone_hz += (int32)(-128.0f * (int32)input_0.up.ended_down);
		game_state_p.x_offset += (int32)(4.8f * (int32)input_0.left.ended_down);
		game_state_p.x_offset += (int32)(-4.8f * (int32)input_0.right.ended_down);
	}

	// Input.AButtonHalfTransitionCount;
	if (input_0.a.ended_down || input_1.a.ended_down)
	{
		game_state_p.y_offset += 1;
	}
	if (input_0.y.ended_down || input_1.y.ended_down)
	{
		game_state_p.y_offset -= 1;
	}

	game_output_sound(sound_buffer_p, game_state_p.tone_hz);
	render_weird_grandent(buffer_p, game_state_p.x_offset, game_state_p.y_offset);
}

void game_input::clear_current_frame()
{
	for (int i = 0; i < ARRAY_COUNT(controllers); ++i) {
		controllers[i].clear_current_frame();
	}
}

void game_controller_input::clear_current_frame()
{
	is_connected = false;
	
	left_stick.average_x = 0;
	left_stick.average_y = 0;

	right_stick.average_x = 0;
	right_stick.average_y = 0;
	
	left_trigger = 0.0f;
	right_trigger = 0.0f;
	up.half_transition_count = 0;
	down.half_transition_count = 0;
	left.half_transition_count = 0;
	right.half_transition_count = 0;
	left_shoulder.half_transition_count = 0;
	right_shoulder.half_transition_count = 0;
	a.half_transition_count = 0;
	b.half_transition_count = 0;
	x.half_transition_count = 0;
	y.half_transition_count = 0;
	start.half_transition_count = 0;
	back.half_transition_count = 0;
}