#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./../minilibx/mlx.h"

#define FALSE 0
#define TRUE 1
#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)
#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

#define MINIMAP_SCALE_FACTOR 0.3

#define FOV_ANGLE (60 * (M_PI / 180))

#define NUM_RAYS WINDOW_WIDTH

// int isGameRunning = FALSE;

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

typedef struct s_player{
	float x;
	float y;
	float width;
	float height;
	int turn_direction;// -1 for left +1 for right
	int walk_direction;// -1 for back +1 fot front
	float rotation_angle;
	float walk_speed;
	float turn_speed;
}	t_player;


typedef struct  s_vars {
        void    *mlx;
        void    *win;
}               t_vars;

typedef struct  s_data {
    void        *img;
    char        *addr;
	t_vars		vars;
	t_player	p;
    int         bits_per_pixel;
    int         line_length;
    int         endian;
}               t_data;


void setup(t_data *img)
{
	img->p.x = WINDOW_WIDTH / 2;
	img->p.y = WINDOW_HEIGHT / 2;
	img->p.width = 5;
	img->p.height = 5;
	img->p.turn_direction = 0;
	img->p.walk_direction = 0;
	img->p.rotation_angle = M_PI / 2;
	img->p.walk_speed = 10;
	img->p.turn_speed = (1 * (M_PI / 180));

}

void            draw_pixel(t_data *data, int x, int y, int color)
{
    char    *dst;

	// printf("x = %d\n y= %d\n", x, y);
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
void move_player(t_data *img)
{
	img->p.rotation_angle += img->p.turn_direction * img->p.turn_speed;

	float move_step = img->p.walk_direction * img->p.walk_speed;

	float new_player_x = img->p.x + cos(img->p.rotation_angle) * move_step;
	float new_player_y = img->p.y + sin(img->p.rotation_angle) * move_step;

	img->p.x = new_player_x;
	img->p.y = new_player_y;

}

void destroyWindow(t_data *data){
	mlx_destroy_window(data->vars.mlx, data->vars.win);
	exit(0);
}

int key_pressed(int keycode, t_data *data){
	printf("keycode = %d\n",keycode);

	if (keycode == 53)//65307
	{
		destroyWindow(data);
		return (0);
	}
	if (keycode == 1) // S
		data->p.walk_direction = -1;
	if (keycode == 13) // W
		data->p.walk_direction = 1;
	// if (keycode == 0) // A
	// 	data->p.lr_direction = -1;
	// if (keycode == 2) // D
	// 	data->p.lr_direction = 1;
	if (keycode == 123) // <-
		data->p.turn_direction = -1;
	if (keycode == 124) // ->
		data->p.turn_direction = 1;
	return (1);
}

int key_released(int keycode, t_data *data)
{
	if (keycode == 1) // S
		data->p.walk_direction = 0;
	if (keycode == 13) // W
		data->p.walk_direction = 0;
	// if (keycode == 0) // A
	// 	data->p.lr_direction = 0;
	// if (keycode == 2) // D
	// 	data->p.lr_direction = 0;
	if (keycode == 123) // <-
		data->p.turn_direction = 0;
	if (keycode == 124) // ->
		data->p.turn_direction = 0;
	return (1);
}

void clear_map(t_data *img){
	int i;
	int j;

	i = 0;
	while(i < WINDOW_HEIGHT){
		j = 0;
		while (j < WINDOW_WIDTH){
			draw_pixel(img, j, i, 0);
			j++;
		}
		i++;
	}
}



void update(t_data *img)
{
	move_player(img);
}

void draw_square(t_data *img, int x, int y, int size, int color)
{
	int i;
	int j;

	i = 0;
	while (i < size)
	{
		j = 0;
		while(j < size)
		{
			// if (playerx + i >= 300 || playery + j >= 300)
				// break ;
			draw_pixel(img, j + y, i + x, color);
			j++;
		}
		i++;
	}

}

void render_map(t_data *img)
{
	for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
        	int tile_x = i * TILE_SIZE;
            int tile_y = j * TILE_SIZE;
            int tile_color = map[i][j] == 1 ? 0xFFFFFF : 0x00;
			draw_square(img, tile_x * MINIMAP_SCALE_FACTOR, tile_y * MINIMAP_SCALE_FACTOR, TILE_SIZE * MINIMAP_SCALE_FACTOR, tile_color);
        }
    }
}

void draw_line(t_data *data, int x0, int y0, int x1, int y1)
{
	double dx;
	double dy;
	double len;
	int i;

	dx = x1 - x0;
	dy = y1 - y0;
	len = (fabs(dx) >= fabs(dy)) ? fabs(dx) : fabs(dy);
	dx /= len;
	dy /= len;
	i = 0;
	while (i < (int)len)
	{
		draw_pixel(data, x0 + (int)(dx * i), y0 + (int)(dy * i), 0xff0000);
		i++;
	}
}


void render_player(t_data *img)
{
	float x = img->p.x * MINIMAP_SCALE_FACTOR, y = img->p.y * MINIMAP_SCALE_FACTOR;
	draw_square(img, (int)y, (int)x, 1, 0xFFFFFF);
	draw_line(img, x, y, x + 40 * cos(img->p.rotation_angle), y + 40 * sin(img->p.rotation_angle));
}

int render(t_data *img){
	// draw_pixel(img , WINDOW_WIDTH, WINDOW_HEIGHT, 255);
	clear_map(img);//draw window black
	update(img);
	//render game objects here
	render_map(img);
	// render_rays();
	render_player(img);

	// draw_square(img);
	mlx_put_image_to_window(img->vars.mlx, img->vars.win, img->img, 0, 0);//putimage
	//TODO:
	//render all game objects for the current frame
	return (0);
}

int main (int argc, char **argv){
    t_data  img;

    if(!initializeWindow(&img))
		exit(0);

	setup(&img);

	// render(&img);
	mlx_hook(img.vars.win, 2, 1L<<0, &key_pressed, &img);
	mlx_hook(img.vars.win, 3, 1L<<1, &key_released, &img);
	mlx_loop_hook(img.vars.mlx, render, &img);

    mlx_loop(img.vars.mlx);
}
