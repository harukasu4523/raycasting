build:
	gcc -L minilibx -lmlx -framework OpenGL -framework AppKit src/*.c -o raycast;

linux: 
	gcc src/*.c ./minilibx/libmlx.a -I include -Lmlx -lXext -lX11 -lm -o raycast;

run:
	./raycast;

clean:
	rm raycast;
