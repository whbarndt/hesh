void hesh_loop()
{
    char *line;
    char **args;
    int status;

    do {
        printf("$ ");
        //line = hest_read_line();
        //args = hesh_split_line(line);
        //status = hesh_execute(args);
        wait();
        //free(line);
        //free(args);
    } while (status);
};

int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  hesh_loop();

  // Perform any shutdown/cleanup.

  return 0;
}