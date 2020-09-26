#include "platform/glfw.h"

static GLFWwindow* window;
static GLubyte screen[SCREEN_HEIGHT][SCREEN_WIDTH][3];
static COLOR color;

static GLuint vertex_array_id;
static GLuint vertex_buffer_id;
static GLuint index_buffer_id;
static GLuint texture_id;
static GLuint vertex_shader_id;
static GLuint fragment_shader_id;
static GLuint shader_program_id;
static CHIP8* gfx_chip8;

static GLuint index_buffer[] = {0, 1, 2, 2, 3, 0};

static GLfloat vertex_buffer[] = {-1.0f, 1.0f,  0.0f, 0.0f, 0.0f,   // vertex 1
                                  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,   // vertex 2
                                  1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,   // vertex 3
                                  -1.0f, -1.0f, 0.0f, 0.0f, 1.0f};  // vertex 4

static void paint_pixel(GLuint row, GLuint col, COLOR* color) {
  screen[row][col][0] = color->r;
  screen[row][col][1] = color->g;
  screen[row][col][2] = color->b;
}

static void paint_cell(GLuint row, GLuint col, COLOR* color) {
  GLuint pixel_row, pixel_col, drow, dcol;

  pixel_row = row * DISPLAY_PIXEL_SIZE;
  pixel_col = col * DISPLAY_PIXEL_SIZE;

  for (drow = 0; drow < DISPLAY_PIXEL_SIZE; drow++) {
    for (dcol = 0; dcol < DISPLAY_PIXEL_SIZE; dcol++) {
      paint_pixel(pixel_row + drow, pixel_col + dcol, color);
    }
  }
}

static COLOR* attach_color(GLubyte color_id) {
  switch (color_id) {
    case COLOR_1:
      color.r = COLOR_1_R;
      color.g = COLOR_1_G;
      color.b = COLOR_1_B;
      break;
    case COLOR_2:
      color.r = COLOR_2_R;
      color.g = COLOR_2_G;
      color.b = COLOR_2_B;
      break;
    default:
      color.r = COLOR_2_R;
      color.g = COLOR_2_G;
      color.b = COLOR_2_B;
  }

  return &color;
}

static void set_large_pixels(uint8_t* display_buffer) {
  GLuint row, col;
  GLubyte color_id;
  COLOR* color;

  for (row = 0; row < DISPLAY_HEIGHT; row++) {
    for (col = 0; col < DISPLAY_WIDTH; col++) {
      color_id = display_buffer[row * DISPLAY_WIDTH + col] ? COLOR_1 : COLOR_2;
      color = attach_color(color_id);
      paint_cell(row, col, color);
    }
  }
}

static void render() {
  GLuint positionLocation = glGetAttribLocation(shader_program_id, "iPosition");
  GLuint uvLocation = glGetAttribLocation(shader_program_id, "vuv");
  GLuint textureLocation = glGetUniformLocation(shader_program_id, "texture");
  GLuint timeLocation = glGetUniformLocation(shader_program_id, "iTime");
  GLint resolutionLocation =
      glGetUniformLocation(shader_program_id, "iResolution");

  glBindVertexArray(vertex_array_id);

  // xyz vertex
  glEnableVertexAttribArray(positionLocation);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
  glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                        sizeof(GLfloat) * 5, 0);

  // uv vertex
  glEnableVertexAttribArray(uvLocation);
  glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5,
                        (void*)(sizeof(GLfloat) * 3));

  // texture
  glUniform1i(textureLocation, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB,
                  GL_UNSIGNED_BYTE, screen);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glEnable(GL_TEXTURE_2D);

  double time = glfwGetTime();
  glUniform1f(timeLocation, (GLfloat)time);

  glUniform2f(resolutionLocation, SCREEN_WIDTH, SCREEN_HEIGHT);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, index_buffer);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glDisableVertexAttribArray(positionLocation);
  glDisableVertexAttribArray(uvLocation);
}

static void create_window() {
  window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE, NULL, NULL);

  if (!window) {
    error("Unable to create GLFW window.");
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
}

static void setup_vertex_buffer() {
  glGenBuffers(1, &vertex_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer), vertex_buffer,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void setup_index_buffer() {
  glGenBuffers(1, &index_buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer), index_buffer,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void setup_vao() {
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  setup_vertex_buffer();
  setup_index_buffer();

  glBindVertexArray(vertex_array_id);
}

static void setup_texture() {
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, screen);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

static GLchar* load_shader_file(const GLchar* file_path) {
  FILE* fd;
  GLchar* rom_buffer;
  size_t fr;
  uint16_t file_size;

  fd = fopen(file_path, "r");
  if (fd == NULL) error("Unable to open file: %s.", file_path);

  fseek(fd, 0, SEEK_END);
  file_size = ftell(fd);
  rewind(fd);

  rom_buffer = (GLchar*)malloc(sizeof(GLchar) * file_size + 1);
  if (rom_buffer == NULL) return false;

  fr = fread(rom_buffer, sizeof(GLchar), (size_t)file_size, fd);
  if (fr != file_size) return false;
  fclose(fd);

  rom_buffer[file_size] = '\0';

  return rom_buffer;
}

static void setup_vertex_shader() {
  const GLchar* vertex_shader_source;

  vertex_shader_source = load_shader_file("./shaders/chip8.vert");
  vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertex_shader_id, 1, &vertex_shader_source, 0);
  glCompileShader(vertex_shader_id);
}

static void setup_fragment_shader() {
  const GLchar* fragment_shader_source;

  fragment_shader_source = load_shader_file("./shaders/crt.frag");
  fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragment_shader_id, 1, &fragment_shader_source, 0);
  glCompileShader(fragment_shader_id);
}

void GetProgramInfoLog(GLuint shader) {
  GLsizei bufSize;

  glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

  if (bufSize > 1) {
    GLchar* infoLog = (GLchar*)malloc(bufSize);

    if (infoLog != NULL) {
      GLsizei length;

      glGetProgramInfoLog(shader, bufSize, &length, infoLog);
      debug_print("InfoLog: %s", infoLog);
      free(infoLog);
    } else
      error("Could not allocate InfoLog buffer.");
  }
}

static void setup_shaders() {
  setup_vertex_shader();
  setup_fragment_shader();

  shader_program_id = glCreateProgram();
  glAttachShader(shader_program_id, vertex_shader_id);
  glAttachShader(shader_program_id, fragment_shader_id);
  glLinkProgram(shader_program_id);

  GLint success;
  glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
  GetProgramInfoLog(shader_program_id);

  glUseProgram(shader_program_id);
}

void gfx_key_input(GLFWwindow* window, int key, int scancode, int action,
                   int mods) {
  const char* key_name = glfwGetKeyName(key, scancode);
  if (key_name == NULL) return;

  if (action == GLFW_PRESS) {
    keypress(gfx_chip8, *key_name);
  } else if (action == GLFW_RELEASE) {
    keyrelease(gfx_chip8, *key_name);
  }
}

void window_size_callback(GLFWwindow* window, int width, int height) {
  debug_print("Window resized.\n");
}

void window_close_callback(GLFWwindow* window) {
  glfwSetWindowShouldClose(window, GL_TRUE);
  debug_print("Window closed.\n");
}

bool gfx_closed() { return (bool)glfwWindowShouldClose(window); }

void gfx_setup(CHIP8* chip8) {
  gfx_chip8 = chip8;

  if (glfwInit() == GL_FALSE) {
    error("Unable to initialize GLFW.");
    exit(EXIT_FAILURE);
  }

  create_window();

  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    error("Unable to initialize GLEW.");
    exit(EXIT_FAILURE);
  }

  glfwSetTime(0.0);
  glfwSetKeyCallback(window, gfx_key_input);
  glfwSetWindowSizeCallback(window, window_size_callback);
  glfwSetWindowCloseCallback(window, window_close_callback);

  setup_vao();
  setup_texture();
  setup_shaders();

  glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
  atexit(glfwTerminate);
}

void gfx_draw(uint8_t* display_buffer) {
  glClear(GL_COLOR_BUFFER_BIT);

  set_large_pixels(display_buffer);

  render();

  glfwSwapBuffers(window);
  glfwPollEvents();
}

void gfx_terminate() {
  glfwDestroyWindow(window);

  glDeleteBuffers(1, &vertex_buffer_id);
  glDeleteBuffers(1, &index_buffer_id);
  glDeleteVertexArrays(1, &vertex_array_id);
  glDeleteTextures(1, &texture_id);

  glDeleteProgram(shader_program_id);
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);
}
