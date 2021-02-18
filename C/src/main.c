#include <stdio.h>
#include <stdlib.h>
#include "./../minilibx/mlx.h"

#define FALSE 0
#define TRUE 1
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int isGameRunning = FALSE;


typedef struct  s_data {
    void        *img;
    char        *addr;
    int         bits_per_pixel;
    int         line_length;
    int         endian;
}               t_data;

typedef struct  s_vars {
        void    *mlx;
        void    *win;
}               t_vars;

void            draw_pixel(t_data *data, int x, int y, int color)
{
    char    *dst;

    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = color;
}

int initializeWindow(t_data *img, t_vars *vars){
    vars->mlx = mlx_init();
	// 	printf("minilibx error \n");
	// 	return (FALSE);
	// }
    vars->win = mlx_new_window(vars->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "udemy_cub");
	if (!vars->win){
		printf("error creating mlx window\n");
		return (FALSE);
	}
    img->img = mlx_new_image(vars->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (!img->img){
		printf("error init mlx img\n");
		return (FALSE);
	}
    img->addr = mlx_get_data_addr(img->img, &img->bits_per_pixel, &img->line_length, &img->endian);
    mlx_put_image_to_window(vars->mlx, vars->win, img->img, 0, 0);

	return (TRUE);
}

void destroyWindow(t_vars *vars){
	mlx_destroy_window(vars->mlx, vars->win);
	exit(0);
}

int key_pressed(int keycode, t_vars *vars){
	if (keycode == 53){
		// isGameRunning = FALSE;
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

void render(t_data *img, t_vars *vars){
	// draw_pixel(img , WINDOW_WIDTH, WINDOW_HEIGHT, 255);
	clear_map(img);//draw window black
    mlx_put_image_to_window(vars->mlx, vars->win, img->img, 0, 0);//putimage

	//TODO:
	//render all game objects for the current frame

}

int main (int argc, char **argv){
	t_vars vars;
    t_data  img;

    isGameRunning = initializeWindow(&img, &vars);

	// setup();

	while (isGameRunning){
		printf("aaaaaaaa\n");
		// update();
		render(&img, &vars);
		mlx_hook(vars.win, 2, 1L<<0, &key_pressed, &vars);
		isGameRunning = FALSE;
	}
	destroyWindow(&vars);
    mlx_loop(vars.mlx);
}
