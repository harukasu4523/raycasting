#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include<stdbool.h>
#include "./../minilibx/mlx.h"
#include "../textures.h"
// #include "../wall_s.xpm"

#define FALSE 0
#define TRUE 1
#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)
#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20
#define NUM_TEXTURES 8


#define TEX_WIDTH 64
#define TEX_HEIGHT 64


uint32_t* walltexture = NULL;
uint32_t* textures[NUM_TEXTURES];



#define MINIMAP_SCALE_FACTOR 0.3

#define FOV_ANGLE (60 * (M_PI / 180))

#define NUM_RAYS WINDOW_WIDTH

// int isGameRunning = FALSE;

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 4, 0, 4, 0, 2, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 3, 3, 0, 0, 2},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 2, 2, 2, 2, 2, 2}
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

typedef struct s_tmp{
	float horzx;
	float horzy;
	float vertx;
	float verty;
	bool hit_horzsprite;
	bool hit_vertsprite;
}			t_tmp;

typedef struct	s_sprite{
	float x;
	float y;
	float distance;
	float angle;
	int texture;
	bool side;
	bool hit;
}				t_sprite[NUM_RAYS];



typedef struct s_texture{
	void	*img;
	char	*addr;
	int		*data;
	int		bpp;
	int		size_l;
	int		endian;
	int		width;
	int		height;
}				t_texture;

typedef struct s_img{
	void        *img;
	int			*data;
    int         bpp;
    int         size_l;
    int         endian;
	int			img_height;
	int			img_width;

}				t_img;


typedef struct  s_value {
    void	   	*mlx;
    void    	*win;
	t_player	p;
	t_ray		ray;
	t_img		img;
	t_sprite	spr;
	int 		*texture[5];
	int			tex_width[5];
	int			tex_height[5];
}               t_value;

// uint32_t* colorbuffer = NULL;

void setup(t_value *info)
{
	info->p.x = WINDOW_WIDTH / 2;
	info->p.y = WINDOW_HEIGHT / 2;
	info->p.width = 1;
	info->p.height = 1;
	info->p.turn_direction = 0;
	info->p.walk_direction = 0;
	info->p.rotation_angle = M_PI / 2.0;
	info->p.walk_speed = 1;
	info->p.turn_speed = (1.0 * (M_PI / 180));

	// walltexture = (uint32_t*)malloc(sizeof(uint32_t) * TEX_WIDTH * TEX_HEIGHT);
	// // create a texture with a pattern of blue or black
	// for(int x = 0; x < TEX_WIDTH; x++)
	// {
	// 	for(int y = 0; y < TEX_HEIGHT; y++)
	// 	{
	// 		walltexture[TEX_WIDTH * y + x] = (x % 8 && y % 8) ?  0x0000ff : 0x000000;
	// 	}
	// }

}

void            draw_pixel(t_img *img, int x, int y, int color)
{
    char    *dst;


    dst = (char *)img->data + (y * img->size_l + x * (img->bpp / 8));
    *(unsigned int*)dst = color;
}

int initializeWindow(t_value *info){
    // info->mlx = mlx_init();
    info->win = mlx_new_window(info->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "udemy_cub");
	if (!info->win){
		printf("error creating mlx window\n");
		return (FALSE);
	}
    info->img.img = mlx_new_image(info->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (!info->img.img){
		printf("error init mlx img\n");
		return (FALSE);
	}
    info->img.data = (int*)mlx_get_data_addr(info->img.img, &(info->img.bpp), &(info->img.size_l), &(info->img.endian));

	return (TRUE);
}

int map_has_wall_at(float x,float y)
{
	int wall_at;

    if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
        return 1;
    }
    int mapGridIndexX = floor(x / TILE_SIZE);
    int mapGridIndexY = floor(y / TILE_SIZE);
	wall_at = map[mapGridIndexY][mapGridIndexX];
    return (!(wall_at == 0 || wall_at == 5));
}

void move_player(t_value *info)
{
	info->p.rotation_angle += info->p.turn_direction * info->p.turn_speed;

	float move_step = info->p.walk_direction * info->p.walk_speed;

	float new_player_x = info->p.x + cos(info->p.rotation_angle) * move_step;
	float new_player_y = info->p.y + sin(info->p.rotation_angle) * move_step;

	if(!map_has_wall_at(new_player_x, new_player_y))
	{
		info->p.x = new_player_x;
		info->p.y = new_player_y;
	}
}

void destroyWindow(t_value *info){
	mlx_destroy_window(info->mlx, info->win);
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


int key_pressed(int keycode, t_value *v){
	printf("keycode = %d\n",keycode);

	if (keycode == 53)//esc
	{
		destroyWindow(v);
		return (0);
	}
	if (keycode == 1) // S
		v->p.walk_direction = -1;
	if (keycode == 13) // W
		v->p.walk_direction = 1;
	if (keycode == 123) // <-
		v->p.turn_direction = -1;
	if (keycode == 124) // ->
		v->p.turn_direction = 1;
	return (1);
}

int key_released(int keycode, t_value *v)
{
	if (keycode == 1) // S
		v->p.walk_direction = 0;
	if (keycode == 13) // W
		v->p.walk_direction = 0;
	if (keycode == 123) // <-
		v->p.turn_direction = 0;
	if (keycode == 124) // ->
		v->p.turn_direction = 0;
	return (1);
}

void clear_map(t_value *info){
	int i;
	int j;

	i = 0;
	while(i < WINDOW_HEIGHT){
		j = 0;
		while (j < WINDOW_WIDTH){
			draw_pixel(&info->img, i, j, 0);
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

int get_spriteData(t_tmp *tmp, float x, float y, int horz)
{
	// tmp.hit_horzsprite = false;
	// tmp.hit_vertsprite = false;
	if(horz){
		printf("check1\n");
		tmp->horzx = x;
		tmp->horzy = y;
		tmp->hit_horzsprite = true;
	}else{
		printf("check2\n");
		tmp->vertx = x;
		tmp->verty = y;
		tmp->hit_vertsprite = true;
	}
	return (1);
}

void calc_spriteDistance(t_value *info, t_tmp tmp, int i, float angle)
{
	// printf("check2\n");
	float horz_distance;
	float vert_distance;

	// printf("%d, %d\n",tmp.hit_horzsprite,tmp.hit_vertsprite);
	horz_distance = tmp.hit_horzsprite ? distanceBetweenPoints(info->p.x, info->p.y, tmp.horzx, tmp.horzy) : FLT_MAX;
	vert_distance = tmp.hit_vertsprite ? distanceBetweenPoints(info->p.x, info->p.y, tmp.vertx, tmp.verty) : FLT_MAX;

	if (horz_distance < vert_distance)
	{
		info->spr[i].distance = horz_distance;
		info->spr[i].x = tmp.horzx;
		info->spr[i].y = tmp.horzy;
		info->spr[i].side = false;
	}else
	{
		info->spr[i].distance = vert_distance;
		info->spr[i].x = tmp.vertx;
		info->spr[i].y = tmp.verty;
		info->spr[i].side = true;	
	}
	info->spr[i].hit = true;
	info->spr[i].angle = angle;

}

void cast_ray(float ray_angle, int strip_id, t_value *info)
{
	t_tmp tmp;
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
	yintercept = floor(info->p.y / TILE_SIZE) * TILE_SIZE;
	yintercept += face_down ? TILE_SIZE : 0;

	// Find the x-coordinate of the closest horizontal grid intersection
	xintercept = info->p.x + (yintercept - info->p.y) / tan(ray_angle);

	// Calculate the increment xstep and ystep
	ystep = TILE_SIZE;
	ystep *= face_up ? -1 : 1;

	xstep = TILE_SIZE / tan(ray_angle);
	xstep *= (face_left && xstep > 0) ? -1 : 1;
	xstep *= (face_right && xstep < 0) ? -1 : 1;

	float next_horz_touch_x = xintercept;
	float next_horz_touch_y = yintercept;
	int sprite = 0;

	// Increment xstep and ystep until we find a wall
	while (next_horz_touch_x >= 0 && next_horz_touch_x <= WINDOW_WIDTH && next_horz_touch_y >= 0 && next_horz_touch_y <= WINDOW_HEIGHT) {
		float x_to_check = next_horz_touch_x;
		float y_to_check = next_horz_touch_y + (face_up ? -1 : 0);

		if (map[(int)(y_to_check / TILE_SIZE)][(int)(x_to_check / TILE_SIZE)] == 5)
			sprite = get_spriteData(&tmp, next_horz_touch_x, next_horz_touch_y, 1);
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
	xintercept = floor(info->p.x / TILE_SIZE) * TILE_SIZE;
	xintercept += face_right ? TILE_SIZE : 0;

	// Find the x-coordinate of the closest horizontal grid intersection
	yintercept = info->p.y + (xintercept - info->p.x) * tan(ray_angle);

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
		if (map[(int)(y_to_check / TILE_SIZE)][(int)(x_to_check / TILE_SIZE)] == 5)
			sprite = get_spriteData(&tmp, next_horz_touch_x, next_horz_touch_y, 0);
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
        ? distanceBetweenPoints(info->p.x, info->p.y, horz_wall_hit_x, horz_wall_hit_y)
        : FLT_MAX;
    float vert_hit_distance = found_vert_wall_hit
        ? distanceBetweenPoints(info->p.x, info->p.y, vert_wall_hit_x, vert_wall_hit_y)
        : FLT_MAX;
	// info->spr[strip_id].hit = false;
	if (sprite)
		calc_spriteDistance(info, tmp, strip_id, ray_angle);

    if (vert_hit_distance < horz_hit_distance) {
        info->ray[strip_id].distance = vert_hit_distance;
        info->ray[strip_id].wall_hit_x = vert_wall_hit_x;
        info->ray[strip_id].wall_hit_y = vert_wall_hit_y;
        info->ray[strip_id].wall_hit_content = vert_wall_content;
        info->ray[strip_id].was_hit_vertical = TRUE;
    } else {
        info->ray[strip_id].distance = horz_hit_distance;
        info->ray[strip_id].wall_hit_x = horz_wall_hit_x;
        info->ray[strip_id].wall_hit_y = horz_wall_hit_y;
        info->ray[strip_id].wall_hit_content = horz_wall_content;
        info->ray[strip_id].was_hit_vertical = FALSE;
    }
    info->ray[strip_id].ray_angle = ray_angle;
    info->ray[strip_id].face_down = face_down;
    info->ray[strip_id].face_up = face_up;
    info->ray[strip_id].face_left = face_left;
    info->ray[strip_id].face_right = face_right;
}

void cast_all_rays(t_value *info)
{
	float ray_angle = info->p.rotation_angle - (FOV_ANGLE / 2);

	for (int strip_id = 0; strip_id < NUM_RAYS; strip_id++)
	{
		cast_ray(ray_angle, strip_id, info);
		ray_angle += FOV_ANGLE / NUM_RAYS;
	}
}

void update(t_value *v)
{
	move_player(v);
	cast_all_rays(v);
}


void draw_square(t_value *info, int x, int y, int size, int color)
{
	int i;
	int j;

	i = 0;
	while (i < size)
	{
		j = 0;
		while(j < size)
		{
			draw_pixel(&info->img, y + j, x + i, color);
			j++;
		}
		i++;
	}

}

void render_map(t_value *v)
{
	for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
        	int tile_x = i * TILE_SIZE;
            int tile_y = j * TILE_SIZE;
            int tile_color = map[i][j] == 0 ?  0x333333 : 0xFFFFFF;
			draw_square(v, tile_x * MINIMAP_SCALE_FACTOR, tile_y * MINIMAP_SCALE_FACTOR, TILE_SIZE * MINIMAP_SCALE_FACTOR, tile_color);
        }
    }
}

void draw_line(t_value *info, float x0, float y0, float x1, float y1, int color)
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
		draw_pixel(&info->img, x0 + (int)(dx * i), y0 + (int)(dy * i), 0xff0000);
		i++;
	}
}

void render_rays(t_value *info)
{
	int color = 0xFF0000;
	for (int i = 0; i < NUM_RAYS; i++)
	{
		draw_line(info, MINIMAP_SCALE_FACTOR * info->p.x,MINIMAP_SCALE_FACTOR * info->p.y, MINIMAP_SCALE_FACTOR * info->ray[i].wall_hit_x , MINIMAP_SCALE_FACTOR * info->ray[i].wall_hit_y, color);
	}
}


void render_player(t_value *info)
{
	float x = info->p.x * MINIMAP_SCALE_FACTOR, y = info->p.y * MINIMAP_SCALE_FACTOR;
	draw_square(info, (int)y, (int)x, 1, 0xFFFFFF);
	draw_line(info, x, y, x + 40 * cos(info->p.rotation_angle), y + 40 * sin(info->p.rotation_angle), 0x0000FF);
}

void render_texture(t_value *info,int i, int tex_num)
{
	printf("%lf\n",info->spr[i].distance);
	if ((int)info->spr[i].distance < 60)
		info->spr[i].distance = 60;
	float prep_distance = info->spr[i].distance * cos(info->spr[i].angle - info->p.rotation_angle);
	float distance_proj_plane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
	float projected_wall_height = (TILE_SIZE / prep_distance) * distance_proj_plane;

	int wall_strip_height = (int)projected_wall_height;
	int wall_top_pixel = (WINDOW_HEIGHT / 2) - (wall_strip_height / 2);
	wall_top_pixel = wall_top_pixel < 0 ? 0 : wall_top_pixel;

	int wall_bottom_pixel = (WINDOW_HEIGHT / 2) + (wall_strip_height / 2);
	wall_bottom_pixel = wall_bottom_pixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wall_bottom_pixel;

	int texture_offset_x;
	printf("%d\n",info->spr[i].side);
	if (info->spr[i].side)
	{
		//perform offset for the vertical
		texture_offset_x = (int)info->spr[i].y % TILE_SIZE;
	}else{
		texture_offset_x = (int)info->spr[i].x % TILE_SIZE;
	}
	for (int y = wall_top_pixel ; y < wall_bottom_pixel; y++)
	{
		//TODO calculate texture offset_y
		int distance_from_top = y + (wall_strip_height / 2) - (WINDOW_HEIGHT / 2);
		int texture_offset_y = distance_from_top * ((float)TEX_HEIGHT / wall_strip_height);
		//set the color of the wall based on the color from the texture
		uint32_t texture_color = info->texture[tex_num][texture_offset_y * info->tex_width[tex_num] + texture_offset_x];
		if (!texture_color)
			continue;
		// printf("aaaa%d\n",texture_color);
		info->img.data[(WINDOW_WIDTH * y) + i] = texture_color;
	}
}


void generate3d_projection(t_value *info)
{
	for(int i = 0; i < NUM_RAYS; i++)
	{
		float prep_distance = info->ray[i].distance * cos(info->ray[i].ray_angle - info->p.rotation_angle);
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
			info->img.data[(WINDOW_WIDTH * y) + i] = 0x333333;
		}
		//TODO calculate texture offset_x
		if (info->ray[i].was_hit_vertical)
		{
			//perform offset for the vertical
			texture_offset_x = (int)info->ray[i].wall_hit_y % TILE_SIZE;
		}else
		{
			//perform offser for the horizontal
			texture_offset_x = (int)info->ray[i].wall_hit_x % TILE_SIZE;
		}
		int tex_num; //= info->ray[i].wall_hit_content - 1;
		// printf("check%d\n",info->spr[i].hit);
		// if (info->spr[i].hit){
		// 	tex_num = 4;
		// }
		if(info->ray[i].face_up && !info->ray[i].was_hit_vertical)
		{
			tex_num = 0;
		}else if(info->ray[i].face_up && info->ray[i].was_hit_vertical)
		{
			if(info->p.x > info->ray[i].wall_hit_x)
				tex_num = 1;
			else
				tex_num = 2;
		}else if (info->ray[i].face_down && !info->ray[i].was_hit_vertical)
		{
			tex_num = 3;
		}else if(info->ray[i].face_down && info->ray[i].was_hit_vertical)
		{
			if(info->p.x < info->ray[i].wall_hit_x)
				tex_num = 2;
			else
				tex_num = 1;
		}
		// printf("%d\n",tex_num);
		// if (tex_num != 4)
		// {
		for (int y = wall_top_pixel ; y < wall_bottom_pixel; y++)
		{
			//TODO calculate texture offset_y
			int distance_from_top = y + (wall_strip_height / 2) - (WINDOW_HEIGHT / 2);
			int texture_offset_y = distance_from_top * ((float)TEX_HEIGHT / wall_strip_height);
			//set the color of the wall based on the color from the texture
			uint32_t texture_color = info->texture[tex_num][texture_offset_y * info->tex_width[tex_num] + texture_offset_x];
			// if (texture_color == 0)
			// 	continue;
			info->img.data[(WINDOW_WIDTH * y) + i] = texture_color;
		}
			// }
		// printf("check%d\n",info->spr[i].hit);
		// int a = info->spr[i].hit;
		//set the color of the floor
		for (int y = wall_bottom_pixel; y < WINDOW_HEIGHT ; y++)
		{
			info->img.data[(WINDOW_WIDTH * y) + i] = 0x777777;
		}
		if (info->spr[i].hit)
			render_texture(info, i, 4);
	}

}

int render(t_value *info){
	clear_map(info);//draw window black
	update(info);
	//Render the wall and sprite
	generate3d_projection(info);
	//renderSpriteProjection

	//display  the minimap
	render_map(info);
	render_rays(info);
	render_player(info);

	// draw_square(info);
	mlx_put_image_to_window(info->mlx, info->win, info->img.img, 0, 0);//putimage

	//TODO:
	//render all game objects for the current frame
	return (0);
}

void texture_in(t_value *info,char *path,int tex_num, t_img *img)
{
	// printf("%d\n",tex_num);

	img->img = mlx_xpm_file_to_image(info->mlx, path, &img->img_width, &img->img_height);
	if (img->img == 0)
	{
		printf("error\n");
		return ;
	}
	info->tex_width[tex_num] = img->img_width;
	info->tex_height[tex_num] = img->img_height;
	info->texture[tex_num] = (int*)malloc(sizeof(int) * img->img_width * img->img_height);
    img->data = (int *)mlx_get_data_addr(img->img, &(img->bpp), &(img->size_l), &(img->endian));
	int y = 0;
	while(y < img->img_height)
	{
		int x = 0;
		while(x < img->img_width)
		{
			info->texture[tex_num][img->img_width * y + x] = img->data[(img->size_l / 4) * y + x];
			x++;
		}
		y++;
	}
}

void load_texture(t_value *info, t_img *img)
{
	texture_in(info, "./textures/eagle.xpm", 0, img);
	texture_in(info, "./textures/mossy.xpm", 1, img);
	texture_in(info, "./textures/greenlight.xpm", 2, img);
	texture_in(info, "./textures/pillar.xpm", 3, img);
	texture_in(info, "./textures/barrel.xpm", 4, img);
}

int main (int argc, char **argv){
    t_value  info;
	t_img img;
    info.mlx = mlx_init();
	load_texture(&info,&img);
    if(!initializeWindow(&info))
		exit(0);
	setup(&info);
	mlx_hook(info.win, 2, 1L<<0, &key_pressed, &info);
	mlx_hook(info.win, 3, 1L<<1, &key_released, &info);
	mlx_loop_hook(info.mlx, render, &info);

    mlx_loop(info.mlx);
}
