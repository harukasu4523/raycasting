#include <stdio.h>
#include <stdlib.h>
#include "./../minilibx/mlx.h"

#define FALSE 0
#define TRUE 1
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// int isGameRunning = FALSE;
int playerx, playery;


typedef struct  s_vars {
        void    *mlx;
        void    *win;
}               t_vars;

typedef struct  s_data {
    void        *img;
    char        *addr;
	t_vars		vars;
    int         bits_per_pixel;
    int         line_length;
    int         endian;
}               t_data;


void setup()
{
	playerx = 0;
	playery = 0;
}

void            draw_pixel(t_data *data, int x, int y, int color)
{
    char    *dst;

    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = color;
}

int initializeWindow(t_data *data){
    data->vars.mlx = mlx_init();
	// 	printf("minilibx error \n");
	// 	return (FALSE);
	// }
    data->vars.win = mlx_new_window(data->vars.mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "udemy_cub");
	if (!data->vars.win){
		printf("error creating mlx window\n");
		return (FALSE);
	}
    data->img = mlx_new_image(data->vars.mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (!data->img){
		printf("error init mlx img\n");
		return (FALSE);
	}
    data->addr = mlx_get_data_addr(data->img, &(data->bits_per_pixel), &(data->line_length), &(data->endian));
    mlx_put_image_to_window(data->vars.mlx, data->vars.win, data->img, 0, 0);

	return (TRUE);
}

void destroyWindow(t_data *data){
	mlx_destroy_window(data->vars.mlx, data->vars.win);
	exit(0);
}

int key_pressed(int keycode, t_data *data){
	printf("keycode = %d\n",keycode);
	
	if (keycode == 65307){
		destroyWindow(data);
		return (0);
	}
	return (1);
}
void clear_map(t_data *img){
	int i;
	int j;

	i = 0;
	while(i < 200){
		j = 0;
		while (j < 300){
			draw_pixel(img, j, i, 0);
			j++;
		}
		i++;
	}
}
void update()
{
	playerx += 1;
	playery += 1;
}

void draw_square(t_data *img)
{
	int i;
	int j;

	i = 0;
	while (i < 20)
	{
		j = 0;
		while(j < 20)
		{
			draw_pixel(img, j + playery , i + playerx , 0xFF0000);
			j++;
		}
		i++;
	}

}

int render(t_data *img){
	// draw_pixel(img , WINDOW_WIDTH, WINDOW_HEIGHT, 255);
	clear_map(img);//draw window black
	update();
	draw_square(img);
	mlx_put_image_to_window(img->vars.mlx, img->vars.win, img->img, 0, 0);//putimage
	//TODO:
	//render all game objects for the current frame
	return (0);
}

int main (int argc, char **argv){
    t_data  img;

    if(!initializeWindow(&img))
		exit(0);

	setup();

	// render(&img);
	mlx_hook(img.vars.win, 2, 1L<<0, &key_pressed, &img);
	mlx_loop_hook(img.vars.win, render, &img);
    mlx_loop(img.vars.mlx);
}
