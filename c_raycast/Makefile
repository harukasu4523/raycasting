build:
	gcc -L minilibx -lmlx -framework OpenGL -framework AppKit src/sprite_main.c -o raycast;

linux: 
	gcc src/*.c ./minilibx/libmlx.a -I include -Lmlx -lXext -lX11 -lm -o raycast;

test:
	gcc -L minilibx -lmlx -framework OpenGL -framework AppKit test.c -o raycast;

run:
	./raycast;

clean:
	rm raycast;
