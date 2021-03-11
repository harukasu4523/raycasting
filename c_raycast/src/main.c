#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "./../minilibx/mlx.h"

#define FALSE 0
#define TRUE 1
#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)
#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20


#define TEX_WIDTH 64
#define TEX_HEIGHT 64


uint32_t* walltexture = NULL;





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

typedef struct s_ray{
	float ray_angle;
	float wall_hit_x;
	float wall_hit_y;
	float distance;
	int was_hit_vertical;
	int face_down;
	int face_up;
	int face_left;
	int face_right;
	int wall_hit_content;
}				t_ray[NUM_RAYS];


typedef struct  s_vars {
        void    *mlx;
        void    *win;
}               t_vars;

typedef struct  s_data {
    void        *img;
	void		*ptr;
    char        *addr;
	int 		*data;
	t_vars		vars;
	t_player	p;
	t_ray		ray;
	int			width;
	int			height;
    int         bits_per_pixel;
    int         line_length;
    int         endian;
}               t_data;

// uint32_t* colorbuffer = NULL;

void setup(t_data *img)
{
	img->p.x = WINDOW_WIDTH / 2;
	img->p.y = WINDOW_HEIGHT / 2;
	img->p.width = 1;
	img->p.height = 1;
	img->p.turn_direction = 0;
	img->p.walk_direction = 0;
	img->p.rotation_angle = M_PI / 2.0;
	img->p.walk_speed = 1;
	img->p.turn_speed = (1.0 * (M_PI / 180));

	img->data = (int *)malloc(sizeof(int) * WINDOW_WIDTH * WINDOW_HEIGHT);

	walltexture = (uint32_t*)malloc(sizeof(uint32_t) * TEX_WIDTH * TEX_HEIGHT);
	// create a texture with a pattern of blue or black
	for(int x = 0; x < TEX_WIDTH; x++)
	{
		for(int y = 0; y < TEX_HEIGHT; y++)
		{
			walltexture[TEX_WIDTH * y + x] = (x % 8 && y % 8) ?  0x0000ff : 0x000000;
		}
	}
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

int map_has_wall_at(float x,float y)
{
    if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
        return 1;
    }
    int mapGridIndexX = floor(x / TILE_SIZE);
    int mapGridIndexY = floor(y / TILE_SIZE);
    return map[mapGridIndexY][mapGridIndexX] != 0;
}

void move_player(t_data *img)
{
	img->p.rotation_angle += img->p.turn_direction * img->p.turn_speed;

	float move_step = img->p.walk_direction * img->p.walk_speed;

	float new_player_x = img->p.x + cos(img->p.rotation_angle) * move_step;
	float new_player_y = img->p.y + sin(img->p.rotation_angle) * move_step;

	if(!map_has_wall_at(new_player_x, new_player_y))
	{
		img->p.x = new_player_x;
		img->p.y = new_player_y;
	}
}

void destroyWindow(t_data *data){
	mlx_destroy_window(data->vars.mlx, data->vars.win);
	exit(0);
}


/////////////////////////
//linux keycode
////////////////////////

// int key_pressed(int keycode, t_data *data){
// 	printf("keycode = %d\n",keycode);

// 	if (keycode == 65307)//53
// 	{
// 		destroyWindow(data);
// 		return (0);
// 	}
// 	if (keycode == 115)//1) // S
// 		data->p.walk_direction = -1;
// 	if (keycode == 119)//13) // W
// 		data->p.walk_direction = 1;
// 	// if (keycode == 0) // A
// 	// 	data->p.lr_direction = -1;
// 	// if (keycode == 2) // D
// 	// 	data->p.lr_direction = 1;
// 	if (keycode == 65361)//123) // <-
// 		data->p.turn_direction = -1;
// 	if (keycode == 65363)//124) // ->
// 		data->p.turn_direction = 1;
// 	return (1);
// }

// int key_released(int keycode, t_data *data)
// {
// 	if (keycode == 115)//1) // S
// 		data->p.walk_direction = 0;
// 	if (keycode == 119)//13) // W
// 		data->p.walk_direction = 0;
// 	// if (keycode == 0) // A
// 	// 	data->p.lr_direction = 0;
// 	// if (keycode == 2) // D
// 	// 	data->p.lr_direction = 0;
// 	if (keycode == 65361)//123) // <-
// 		data->p.turn_direction = 0;
// 	if (keycode == 65363)//124) // ->
// 		data->p.turn_direction = 0;
// 	return (1);
// }

//////////////////////
//mac keycode
//////////////////////


int key_pressed(int keycode, t_data *data){
	printf("keycode = %d\n",keycode);

	if (keycode == 53)//esc
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

float normalize_angle(float angle)
{
	angle = remainder(angle, (M_PI * 2));
		if (angle < 0)
		{
			angle = (M_PI * 2) + angle;
		}
	return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void cast_ray(float ray_angle, int strip_id, t_data *img)
{
	ray_angle = normalize_angle(ray_angle);

	int face_down = ray_angle > 0 && ray_angle < M_PI;
	int face_up = !face_down;

	int face_right = ray_angle < 0.5 * M_PI || ray_angle > 1.5 * M_PI;
	int face_left = !face_right;

	float xintercept, yintercept;
	float xstep, ystep;

	///////////////////////////////////////////
	// HORIZONTAL RAY-GRID INTERSECTION CODE
	///////////////////////////////////////////
	int found_horz_wall_hit = FALSE;
	float horz_wall_hit_x = 0;
	float horz_wall_hit_y = 0;
	int horz_wall_content = 0;


	// Find the y-coordinate of the closest horizontal grid intersenction
	yintercept = floor(img->p.y / TILE_SIZE) * TILE_SIZE;
	yintercept += face_down ? TILE_SIZE : 0;

	// Find the x-coordinate of the closest horizontal grid intersection
	xintercept = img->p.x + (yintercept - img->p.y) / tan(ray_angle);

	// Calculate the increment xstep and ystep
	ystep = TILE_SIZE;
	ystep *= face_up ? -1 : 1;

	xstep = TILE_SIZE / tan(ray_angle);
	xstep *= (face_left && xstep > 0) ? -1 : 1;
	xstep *= (face_right && xstep < 0) ? -1 : 1;

	float next_horz_touch_x = xintercept;
	float next_horz_touch_y = yintercept;

	// Increment xstep and ystep until we find a wall
	while (next_horz_touch_x >= 0 && next_horz_touch_x <= WINDOW_WIDTH && next_horz_touch_y >= 0 && next_horz_touch_y <= WINDOW_HEIGHT) {
		float x_to_check = next_horz_touch_x;
		float y_to_check = next_horz_touch_y + (face_up ? -1 : 0);

		if (map_has_wall_at(x_to_check, y_to_check)) {
			horz_wall_hit_x = next_horz_touch_x;
			horz_wall_hit_y = next_horz_touch_y;
			horz_wall_content = map[(int)floor(y_to_check / TILE_SIZE)][(int)floor(x_to_check / TILE_SIZE)];
			found_horz_wall_hit = TRUE;
			break;
		} else {
			next_horz_touch_x += xstep;
			next_horz_touch_y += ystep;
		}
	}
	/////////////////////////////////////////////
	// VERTICAL RAY-GRID INTERSECTION CODE
	////////////////////////////////////////////


	int found_vert_wall_hit = FALSE;
	float vert_wall_hit_x = 0;
	float vert_wall_hit_y = 0;
	int vert_wall_content = 0;


	// Find the y-coordinate of the closest horizontal grid intersenction
	xintercept = floor(img->p.x / TILE_SIZE) * TILE_SIZE;
	xintercept += face_right ? TILE_SIZE : 0;

	// Find the x-coordinate of the closest horizontal grid intersection
	yintercept = img->p.y + (xintercept - img->p.x) * tan(ray_angle);

	// Calculate the increment xstep and ystep
	xstep = TILE_SIZE;
	xstep *= face_left ? -1 : 1;

	ystep = TILE_SIZE * tan(ray_angle);
	ystep *= (face_up && ystep > 0) ? -1 : 1;
	ystep *= (face_down && ystep < 0) ? -1 : 1;

	float next_vert_touch_x = xintercept;
	float next_vert_touch_y = yintercept;

	// Increment xstep and ystep until we find a wall
	while (next_vert_touch_x >= 0 && next_vert_touch_x <= WINDOW_WIDTH && next_vert_touch_y >= 0 && next_vert_touch_y <= WINDOW_HEIGHT) {
		float x_to_check = next_vert_touch_x + (face_left ? -1 : 0);
		float y_to_check = next_vert_touch_y;

		if (map_has_wall_at(x_to_check, y_to_check)) {
			vert_wall_hit_x = next_vert_touch_x;
			vert_wall_hit_y = next_vert_touch_y;
			vert_wall_content = map[(int)floor(y_to_check / TILE_SIZE)][(int)floor(x_to_check / TILE_SIZE)];
			found_vert_wall_hit = TRUE;
			break;
		} else {
			next_vert_touch_x += xstep;
			next_vert_touch_y += ystep;
		}
	}
	    // Calculate both horizontal and vertical hit distances and choose the smallest one
    float horz_hit_distance = found_horz_wall_hit
        ? distanceBetweenPoints(img->p.x, img->p.y, horz_wall_hit_x, horz_wall_hit_y)
        : FLT_MAX;
    float vert_hit_distance = found_vert_wall_hit
        ? distanceBetweenPoints(img->p.x, img->p.y, vert_wall_hit_x, vert_wall_hit_y)
        : FLT_MAX;

    if (vert_hit_distance < horz_hit_distance) {
        img->ray[strip_id].distance = vert_hit_distance;
        img->ray[strip_id].wall_hit_x = vert_wall_hit_x;
        img->ray[strip_id].wall_hit_y = vert_wall_hit_y;
        img->ray[strip_id].wall_hit_content = vert_wall_content;
        img->ray[strip_id].was_hit_vertical = TRUE;
    } else {
        img->ray[strip_id].distance = horz_hit_distance;
        img->ray[strip_id].wall_hit_x = horz_wall_hit_x;
        img->ray[strip_id].wall_hit_y = horz_wall_hit_y;
        img->ray[strip_id].wall_hit_content = horz_wall_content;
        img->ray[strip_id].was_hit_vertical = FALSE;
    }
    img->ray[strip_id].ray_angle = ray_angle;
    img->ray[strip_id].face_down = face_down;
    img->ray[strip_id].face_up = face_up;
    img->ray[strip_id].face_left = face_left;
    img->ray[strip_id].face_right = face_right;
}

void cast_all_rays(t_data *img)
{
	float ray_angle = img->p.rotation_angle - (FOV_ANGLE / 2);
	for (int strip_id = 0; strip_id < NUM_RAYS; strip_id++)
	{
		cast_ray(ray_angle, strip_id, img);
		ray_angle += FOV_ANGLE / NUM_RAYS;
	}
}

void update(t_data *img)
{
	move_player(img);
	cast_all_rays(img);
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
            int tile_color = map[i][j] == 1 ? 0xFFFFFF : 0x333333;
			draw_square(img, tile_x * MINIMAP_SCALE_FACTOR, tile_y * MINIMAP_SCALE_FACTOR, TILE_SIZE * MINIMAP_SCALE_FACTOR, tile_color);
        }
    }
}

void draw_line(t_data *data, float x0, float y0, float x1, float y1, int color)
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
		draw_pixel(data, x0 + (int)(dx * i), y0 + (int)(dy * i), color);
		i++;
	}
}

void render_rays(t_data *img)
{
	int color = 0xFF0000;
	for (int i = 0; i < NUM_RAYS; i++)
	{
		draw_line(img, MINIMAP_SCALE_FACTOR * img->p.x,MINIMAP_SCALE_FACTOR * img->p.y, MINIMAP_SCALE_FACTOR * img->ray[i].wall_hit_x , MINIMAP_SCALE_FACTOR * img->ray[i].wall_hit_y, color);
	}
}


void render_player(t_data *img)
{
	float x = img->p.x * MINIMAP_SCALE_FACTOR, y = img->p.y * MINIMAP_SCALE_FACTOR;
	draw_square(img, (int)y, (int)x, 1, 0xFFFFFF);
	draw_line(img, x, y, x + 40 * cos(img->p.rotation_angle), y + 40 * sin(img->p.rotation_angle), 0x0000FF);
}

void clear_color_buffer(t_data *img)
{
	img->data = (int *)mlx_get_data_addr(img->img, &(img->bits_per_pixel), &(img->line_length), &(img->endian));
	for(int x = 0; x < WINDOW_WIDTH; x++)
	{
		for(int y = 0 ; y < WINDOW_HEIGHT; y++)
		{
			img->data[(WINDOW_WIDTH * y) + x] = 0x000000;
		}
	}
	// mlx_put_image_to_window(img->vars.mlx, img->vars.win, img->img, 0, 0);//putimage
}

void render_color_bufer(t_data *img)
{
	// img->data = (int *)mlx_get_data_addr(img->img, &(img->bits_per_pixel), &(img->line_length), &(img->endian));
	for(int x = 0; x < WINDOW_WIDTH; x++)
	{
		for(int y = 0 ; y < WINDOW_HEIGHT; y++)
		{
			img->data[(WINDOW_WIDTH * y) + x] = 0x000000;
		}
	}
}

void generate3d_projection(t_data *img)
{
	// img->ptr = mlx_xpm_file_to_image(img->vars.mlx, "../wall_s.xpm", &img->width, &img->height);

	// img->data = (int *)mlx_get_data_addr(img->img, &(img->bits_per_pixel), &(img->line_length), &(img->endian));

	for(int i = 0; i < NUM_RAYS; i++)
	{
		float prep_distance = img->ray[i].distance * cos(img->ray[i].ray_angle - img->p.rotation_angle);
		float distance_proj_plane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
		float projected_wall_height = (TILE_SIZE / prep_distance) * distance_proj_plane;

		int wall_strip_height = (int)projected_wall_height;
		int wall_top_pixel = (WINDOW_HEIGHT / 2) - (wall_strip_height / 2);
		wall_top_pixel = wall_top_pixel < 0 ? 0 : wall_top_pixel;

		int wall_bottom_pixel = (WINDOW_HEIGHT / 2) + (wall_strip_height / 2);
		wall_bottom_pixel = wall_bottom_pixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wall_bottom_pixel;

		int texture_offset_x;
		// set the color of the ceiling
		for (int y = 0; y < wall_top_pixel; y++)
		{
			img->data[(WINDOW_WIDTH * y) + i] = 0x333333;
		}
		//TODO calculate texture offset_x
		if (img->ray[i].was_hit_vertical)
		{
			//perform offset for the vertical
			texture_offset_x = (int)img->ray[i].wall_hit_y % TILE_SIZE;
		}else
		{
			//perform offser for the horizontal
			texture_offset_x = (int)img->ray[i].wall_hit_x % TILE_SIZE;
		}

		for (int y = wall_top_pixel ; y < wall_bottom_pixel; y++)
		{
			//TODO calculate texture offset_y
			int distance_from_top = y + (wall_strip_height / 2) - (WINDOW_HEIGHT / 2)
			int texture_offset_y = distance_from_top * ((float)TEX_HEIGHT / wall_strip_height);
			//set the color of the wall based on the color from the texture
			uint32_t texture_color = walltexture [(TEX_WIDTH * texture_offset_y) + texture_offset_x];
			img->data[(WINDOW_WIDTH * y) + i] = texture_color;
		}
		//set the color of the floor
		for (int y = wall_bottom_pixel; y < WINDOW_HEIGHT ; y++)
		{
			img->data[(WINDOW_WIDTH * y) + i] = 0x777777;
		}

	}
	// mlx_put_image_to_window(img->vars.mlx, img->vars.win, img->ptr, 0, 0);//putimage

}

int render(t_data *img){
	// draw_pixel(img , WINDOW_WIDTH, WINDOW_HEIGHT, 255);
	clear_map(img);//draw window black
	update(img);

	clear_color_buffer(img);
	render_color_bufer(img);
	generate3d_projection(img);
	//clear the color buffer

	//display  the minimap
	render_map(img);
	render_rays(img);
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
