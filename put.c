#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libmemcached/memcached.h>

int main(int argc, char **argv)
{
	struct memcached_st *memc;
	struct memcached_server_st *servers;
	memcached_return rc;
	char *server = argv[1];
	char *key = argv[2];
	char *filename = argv[3];
	struct stat st;
	int fd;
	char *buf = NULL;

	if (argc < 3) {
		fprintf(stderr, "more args required\n");
		return -1;
	}

	memc = memcached_create(NULL);
	servers = memcached_servers_parse(server);
	rc = memcached_server_push(memc, servers);
	if (rc != MEMCACHED_SUCCESS) {
		fprintf(stderr, "memcached_server_push err:%d\n", rc);
		return -1;
	}

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("open");
		rc = fd;
		goto out;
	}
	if (fstat(fd, &st)) {
		perror("fstat");
		rc = -1;
		goto out;
	}
	buf = (char *)malloc(st.st_size);
	if (!buf) {
		perror("malloc");
		rc = -1;
		goto out;
	}
	if ((rc = read(fd, buf, st.st_size)) != st.st_size) {
		printf("read:%d\n", rc);
		goto out;
	}
	close(fd);

	rc = memcached_set(memc, key, strlen(key), buf, st.st_size, (time_t)0, (uint32_t)0);

	if (rc != MEMCACHED_SUCCESS) {
		fprintf(stderr, "memcached_set err:%d\n", rc);
		return -1;
	}

out:
	if (buf)
		free(buf);
	memcached_server_list_free(servers);
	memcached_free(memc);
	return rc;
}
