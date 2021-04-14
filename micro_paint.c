#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BORDER 1
#define INSIDE 2

typedef struct s_matrix
{
	int width;
	int height;
	char color;
	char **array;
}			t_matrix;

typedef struct s_rectangle
{
	char type;
	float x;
	float y;
	float width;
	float height;
	char color;

}			t_rectangle;

int	get_background(FILE *file, t_matrix *matrix)
{
	if(fscanf(file, "%d %d %c\n", &matrix->width, &matrix->height, &matrix->color) == 3)
	{
		if(matrix->width < 1 || matrix->width > 300 || matrix->height < 1 || matrix->height > 300)
			return 1;
		if(!(matrix->array = malloc(matrix->height * sizeof(char *))))
			return 2;
		for(int y = 0; y < matrix->height; y++)
		{
			if(!(matrix->array[y] = malloc(matrix->width)))
				return 2;
			for(int x = 0; x < matrix->width; x++)
				matrix->array[y][x] = matrix->color;
		}
		return 0;
	}
	return 1;
}

int	is_rect(t_rectangle *rect, float x, float y)
{
	if(x >= rect->x && x <= rect->width && y >= rect->y && y <= rect->height)
	{
		if(x - rect->x < 1.0F || y - rect->y < 1.0F || rect->width - x < 1.0F || rect->height - y < 1.0F)
			return BORDER;
		return INSIDE;
	}
	return 0;
}

void	put_one(t_rectangle *rect, t_matrix *matrix)
{
	int ret;

	rect->width += rect->x;
	rect->height += rect->y;
	for(int y = 0; y < matrix->height; y++)
	{
		for(int x = 0; x < matrix->width; x++)
		{
			ret = is_rect(rect, (float)x, (float)y);
			if(ret == BORDER || (ret == INSIDE && rect->type == 'R'))
				matrix->array[y][x] = rect->color;
		}
	}
}

int	get_rectangles(FILE *file, t_matrix *matrix)
{
	t_rectangle rect;
	int ret;

	ret = fscanf(file, "%c %f %f %f %f %c\n", &rect.type, &rect.x, &rect.y, &rect.width, &rect.height, &rect.color);
	while(ret == 6)
	{
		if((rect.type == 'R' || rect.type == 'r') && rect.width > 0 && rect.height > 0)
			put_one(&rect, matrix);
		else
			return 1;
		ret = fscanf(file, "%c %f %f %f %f %c\n", &rect.type, &rect.x, &rect.y, &rect.width, &rect.height, &rect.color);

	}
	if(ret == EOF)
		return 0;
	return 1;
}

void	print_result(t_matrix *matrix)
{
	for(int y = 0; y < matrix->height; y++)
	{
		write(1, matrix->array[y], matrix->width);
		write(1, "\n", 1);
	}
}

int	micropaint(FILE *file)
{
	t_matrix matrix;

	if(get_background(file, &matrix))
		return 1;
	if(get_rectangles(file, &matrix))
		return 1;
	print_result(&matrix);
	return 0;
}

int main(int argc, char **argv)
{
	FILE *file;

	if(argc == 2)
	{
		file = fopen(argv[1], "r");
		if(!file || micropaint(file))
		{
			write(1, "Error: Operation file corrupted\n", 32);
			return 1;
		}
		fclose(file);
		return 0;
	}
	write(1, "Error: argument\n", 16);
	return 1;
}
