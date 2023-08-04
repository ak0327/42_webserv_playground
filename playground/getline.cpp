#include <stdio.h>

int main() {
	char	*line = NULL;
	size_t	len = 0;
	ssize_t	size;

	while (true) {
		size = getline(&line, &len, stdin);
		printf("line:[%s], size:[%zd]\n", line, size);
		if (size == -1)
			break ;
	}
	return 0;
}
