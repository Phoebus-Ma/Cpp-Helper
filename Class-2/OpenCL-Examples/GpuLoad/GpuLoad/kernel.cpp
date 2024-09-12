/**
 * Reference: <https://github.com/ProjectPhysX/OpenCL-Benchmark>.
*/

#include "kernel.h"


string opencl_c_container()
{
    return R() +
        // OpenCL C defines don't work in R() stringification macro.
        "#ifdef cl_khr_fp64" + R(
            kernel void kernel_double(global float *data)
            {
                double x = (double)get_global_id(0);
                double y = (double)get_local_id(0);

                for (uint i = 0u; i < 128u; i++)
                {
                    x = fma(y, x, y);
                    y = fma(x, y, x);
                }

                data[get_global_id(0)] = (float)y;
            }
        ) +
        // cl_khr_fp64.
        "#endif" + R(
            kernel void kernel_float(global float *data)
            {
                float x = (float)get_global_id(0);
                float y = (float)get_local_id(0);

                for (uint i = 0u; i < 512u; i++)
                {
                    x = fma(y, x, y);
                    y = fma(x, y, x);
                }

                data[get_global_id(0)] = y;
            }
        ) +
        // OpenCL C defines don't work in R() stringification macro.
        "#ifdef cl_khr_fp16" + R(
            kernel void kernel_half(global float *data)
            {
                half2 x = (half2)((float)get_global_id(0), (float)get_local_id(0));
                half2 y = (half2)((float)get_local_id(0), (float)get_global_id(0));

                for (uint i = 0u; i < 512u; i++)
                {
                    x = y * x + y;
                    y = x * y + x;
                }

                data[get_global_id(0)] = (float)y.x + (float)y.y;
            }
        ) +
        // cl_khr_fp16.
        "#endif" + R(

            kernel void kernel_long(global float *data)
            {
                long x = (long)get_global_id(0);
                long y = (long)get_local_id(0);

                for (uint i = 0u; i < 8u; i++)
                {
                    x = y * x + y;
                    y = x * y + x;
                }

                data[get_global_id(0)] = as_float((int)y);
            }

            kernel void kernel_int(global float *data)
            {
                int x = get_global_id(0);
                int y = get_local_id(0);

                for (uint i = 0u; i < 512u; i++)
                {
                    x = y * x + y;
                    y = x * y + x;
                }

                data[get_global_id(0)] = as_float(y);
            }

            kernel void kernel_short(global float *data)
            {
                short2 x = as_short2((int)get_global_id(0));
                short2 y = as_short2((int)get_local_id(0));

                for (uint i = 0u; i < 128u; i++)
                {
                    x = y * x + y;
                    y = x * y + x;
                }

                data[get_global_id(0)] = as_float(y);
            }

            kernel void kernel_char(global float *data)
            {
                char4 x = as_char4((int)get_global_id(0));
                char4 y = as_char4((int)get_local_id(0));

                for (uint i = 0u; i < 64u; i++)
                {
                    x = y * x + y;
                    y = x * y + x;
                }

                data[get_global_id(0)] = as_float(y);
            }

            kernel void kernel_coalesced_write(global float *data)
            {
                const uint n = get_global_id(0);

                for (uint i = 0u; i < def_M; i++)
                    data[i * def_N + n] = 0.0f; // coalesced write.
            }
                        
            kernel void kernel_coalesced_read(global float *data)
            {
                const uint n = get_global_id(0);
                float x = 0.0f;

                for (uint i = 0u; i < def_M; i++)
                    x += data[i * def_N + n]; // coalesced read.

                data[n] = x;
            }
                        
            kernel void kernel_misaligned_write(global float *data)
            {
                const uint n = get_global_id(0);

                for (uint i = 0u; i < def_M; i++)
                    data[n * def_M + i] = 0.0f; // misaligned write.
            }
                        
            kernel void kernel_misaligned_read(global float *data)
            {
                const uint n = get_global_id(0);
                float x = 0.0f;

                for (uint i = 0u; i < def_M; i++)
                    x += data[n * def_M + i]; // misaligned read.

                data[n] = x;
            }
        );
}

string get_opencl_c_code()
{
    string r = opencl_c_container();
    r = replace(r, " ", "\n");                  // replace all spaces by new lines.
    r = replace(r, "#ifdef\n", "#ifdef ");      // except for the arguments after some preprocessor options that need to be in the same line.
    r = replace(r, "#ifndef\n", "#ifndef ");
    r = replace(r, "#define\n", "#define ");    // #define with two arguments will not work.
    r = replace(r, "#if\n", "#if ");            // don't leave any spaces in arguments.
    r = replace(r, "#elif\n", "#elif ");        // don't leave any spaces in arguments.
    r = replace(r, "#pragma\n", "#pragma ");

    return "\n" + r;
}
