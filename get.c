#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <libmemcached/memcached.h>

int main(int argc, char **argv)
{
	struct memcached_st *memc;
	struct memcached_server_st *servers;
	memcached_return rc;
	uint16_t flags;
	size_t reslen;
	char *server = argv[1];
	char *key = argv[2];
	char *filename = argv[3];
	int fd;
	char *res;

	if (argc < 4) {
		fprintf(stderr, "more args required\n");
		return -1;
	}

	memc = memcached_create(NULL);
	servers = memcached_servers_parse(server);
	rc = memcached_server_push(memc, servers);
	if (rc != MEMCACHED_SUCCESS) {
		fprintf(stderr, "memcached_server_push err:%d\n", rc);
		goto out;
	}

	res = memcached_get(memc, key, strlen(key), &reslen, &flags, &rc);

	if (rc != MEMCACHED_SUCCESS) {
		fprintf(stderr, "memcached_get err:%d\n", rc);
		goto out;
	}

	fd = open(filename, O_RDWR|O_CREAT);
	if (fd < 0) {
		perror("open");
		rc = fd;
		goto out;
	}
	if ((rc = write(fd, res, reslen)) != reslen) {
		printf("write:%d\n", rc);
		goto out;
	}
	close(fd);

out:
	free(res);
	memcached_server_list_free(servers);
	memcached_free(memc);
	return rc;
}
