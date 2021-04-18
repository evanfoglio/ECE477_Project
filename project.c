/**
 * Author: Jason White
 *
 * Description:
 * Reads joystick/gamepad events and displays them.
 *
 * Compile:
 * gcc joystick.c -o joystick
 *
 * Run:
 * ./joystick [/dev/input/jsX]
 *
 * See also:
 * https://www.kernel.org/doc/Documentation/input/joystick-api.txt
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <wiringPi.h>
//void gpioInit();
void led_modify(int *gpioOut, int numGPIOs, int numLEDs);
void gpioInit(int *gpioOut, int numGPIOs);
#define GPIO2 8
#define GPIO3 9
#define GPIO4 7
#define GPIO17 0
#define GPIO27 2
#define GPIO22 3
#define GPIO10 12
#define GPIO9 13
//#define GPIO27 2



/**
 * Reads a joystick event from the joystick device.
 *
 * Returns 0 on success. Otherwise -1 is returned.
 */
int read_event(int fd, struct js_event *event)
{
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

/**
 * Returns the number of axes on the controller or 0 if an error occurs.
 */
size_t get_axis_count(int fd)
{
    __u8 axes;

    if (ioctl(fd, JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

/**
 * Returns the number of buttons on the controller or 0 if an error occurs.
 */
size_t get_button_count(int fd)
{
    __u8 buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

/**
 * Current state of an axis.
 */
struct axis_state {
    short x, y;
};

/**
 * Keeps track of the current axis state.
 *
 * NOTE: This function assumes that axes are numbered starting from 0, and that
 * the X axis is an even number, and the Y axis is an odd number. However, this
 * is usually a safe assumption.
 *
 * Returns the axis that the event indicated.
 */
size_t get_axis_state(struct js_event *event, struct axis_state axes[3])
{
    size_t axis = event->number / 2;

    if (axis < 3)
    {
        if (event->number % 2 == 0)
            axes[axis].x = event->value;
        else
            axes[axis].y = event->value;
    }

    return axis;
}

int main(int argc, char *argv[])
{
    const char *device;
    int js;
    struct js_event event;
    struct axis_state axes[3] = {0};
    size_t axis;

    device = "/dev/input/js0";
    js = open(device, O_RDONLY);

	if (js == -1)
        	perror("Could not open joystick");



	int gpioOut[8] = {GPIO2, GPIO3, GPIO4, GPIO17, GPIO27, GPIO22, GPIO10, GPIO9};
	gpioInit(gpioOut, 8);


    /* This loop will exit if the controller is unplugged. */
    while (read_event(js, &event) == 0)
    {
        switch (event.type)
        {
            case JS_EVENT_BUTTON:
                printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");
                break;
/*		switch(event.number)
		{
			case 0:
					digitalWrite(GPIO2, event.value);
					break;
			case 1:
					digitalWrite(GPIO3, event.value);
					break;
			case 2:
		                        digitalWrite(GPIO4, event.value);
                                        break;
			case 3:	
                                        digitalWrite(GPIO17, event.value);
                                        break;
			default:
				printf("Not Right Button\n");
				break;
		}
		break;*/
            case JS_EVENT_AXIS:
			axis = get_axis_state(&event, axes);
                	if ((axis < 3)){
                   		//printf("Axis %zu at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);
                		if(axis == 2){
					if(axes[axis].y == -32767)
						led_modify(gpioOut, 8, 0);
					else 
					if(axes[axis].y < -24767)
						led_modify(gpioOut, 8, 1);
					else
					if(axes[axis].y < -16767)
						led_modify(gpioOut, 8, 2);
					else
					if(axes[axis].y < -8767)
						led_modify(gpioOut, 8, 3);
					else
					if(axes[axis].y < 0)
						led_modify(gpioOut, 8, 4);
					else
					if(axes[axis].y < 8767)
						led_modify(gpioOut, 8, 5);
					else
					if(axes[axis].y < 16767)
						led_modify(gpioOut, 8, 6);
					else
					if(axes[axis].y < 24767)
						led_modify(gpioOut, 8, 7);
					else
					if(axes[axis].y == 32767)
						led_modify(gpioOut, 8, 8);
					
				}
				


			}
		break;
            default:
                /* Ignore init events. */
                break;
        }
        
        fflush(stdout);
    }

    close(js);
    return 0;
}


//Intialize GPIO pins being used
void gpioInit(int *gpioOut, int numGPIOs)
{	
	int i;
        wiringPiSetup();
        for(i = numGPIOs; i >= 0; i--){
                pinMode(gpioOut[i], OUTPUT);
//		digitalWrite(gpioOut[i], 1);
        }
}

void led_modify(int *gpioOut, int numGPIOs, int numLEDs)
{

	for(int i = 0; i < numGPIOs; i++)
		if(i < numLEDs)
			digitalWrite(gpioOut[i], 1);
		else
			digitalWrite(gpioOut[i], 0);

}
