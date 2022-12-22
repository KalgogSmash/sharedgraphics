#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SEM_MUTEX_NAME "/sem-mutex"
#define SEM_BUFFER_COUNT_NAME "/sem-buffer-count"
#define SEM_SPOOL_SIGNAL_NAME "/sem-spool-signal"
#define SHARED_MEM_NAME "/posix-shared-mem-example"

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void error (char *msg)
{
    perror (msg);
    exit (1);
}

int main(void)
{
    GLFWwindow* window;
    static const GLfloat fill_color[] = {1.0f, 0.0f, 0.0f, 1.0f};
    static const uint window_height = 480;
    static const uint window_width = 640;
    static const uint buff_size = window_height * window_width * 4;
    static bool x = false;

    float tri_x = 0.0f;
    bool tri_left = true;
    //static const float vertices[] = {
    //    -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.0f,  0.5f, 0.0f
    //};
    sem_t *mutex_sem, *buffer_count_sem, *spool_signal_sem;
    int fd_shm;
    void* shared_mem_ptr;

    static char* pixel_buffer;
    pixel_buffer = (char*)malloc(buff_size);

    if ((mutex_sem = sem_open (SEM_MUTEX_NAME, O_CREAT, 0660, 0)) == SEM_FAILED)
        error ("sem_open");
    // Get shared memory 
    if ((fd_shm = shm_open (SHARED_MEM_NAME, O_RDWR | O_CREAT, 0660)) == -1)
        error ("shm_open");
    if (ftruncate (fd_shm, buff_size) == -1)
       error ("ftruncate");
    if ((shared_mem_ptr = mmap (NULL, buff_size, PROT_READ | PROT_WRITE, MAP_SHARED,
            fd_shm, 0)) == MAP_FAILED)
       error ("mmap");

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(window_width, window_height, "Simple example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental=GL_TRUE;
    GLenum err=glewInit();
    if(err!=GLEW_OK)
    {
        //Problem: glewInit failed, something is seriously wrong.
        std::cout<<"glewInit failed, aborting."<<std::endl;
    }

    glEnable(GL_TEXTURE_2D);

    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, fill_color);

        if (x == true)
        {
            //unsigned int VBO;
            //glGenBuffers(1, VBO);
            //glBindBuffer(GL_ARRAY_BUFFER, VBO);
            //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glBegin(GL_POLYGON);

            glColor3f(1, 0, 0);
            glVertex3f(-0.6, -0.75, 0);

            glColor3f(0, 1, 0);
            glVertex3f(0.6, -0.75, 0);

            glColor3f(0, 0, 1);
            glVertex3f(0, 0.75, 0);

            glEnd();
            glFlush();

            glReadPixels(0, 0, window_width, window_height, GL_RGBA, GL_BYTE, (void*)pixel_buffer);

            //for (uint i = 0; i < buff_size; i+=4)
            //{
            //    char tmp = pixel_buffer[i];
            //    pixel_buffer[i] = pixel_buffer[i+2];
            //    pixel_buffer[i+2] = tmp;    
            //}

            x = false;
        }
        else {
            if (tri_left)
            {
                tri_x -= 0.02f;
                if (tri_x <= -0.5f)
                {
                   tri_left = false; 
                }
            }
            else
            {
               tri_x += 0.02f; 
               if (tri_x >= 0.5f)
                {
                   tri_left = true; 
                }
            }

            glBegin(GL_POLYGON);

            glColor3f(1, 0, 0);
            glVertex3f(-0.6, -0.75, 0);

            glColor3f(0, 1, 0);
            glVertex3f(0.6, -0.75, 0);

            glColor3f(0, 0, 1);
            glVertex3f(tri_x, 0.75, 0);

            glEnd();
            glFlush();

            glReadPixels(0, 0, window_width, window_height, GL_RGBA, GL_BYTE, (void*)pixel_buffer);

            memcpy(shared_mem_ptr, pixel_buffer, buff_size);
            if (sem_post (mutex_sem) == -1)
                error ("sem_post: mutex_sem");

            //glBindTexture(GL_TEXTURE_2D, 0);
//
            //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//
            //glTexImage2D(GL_TEXTURE_2D,
            //    0,
            //    GL_RGBA,
            //    window_width,
            //    window_height,
            //    0,
            //    GL_RGBA,
            //    GL_BYTE,
            //    pixel_buffer);
//
            ////glBindTexture (GL_TEXTURE_2D, 0);
            //glBegin (GL_QUADS);
//
            //glTexCoord2f (0.0, 1.0);
            //glVertex3f (-1.0, 1.0, 0.0);
//
            //glTexCoord2f (1.0, 1.0);
            //glVertex3f (1.0, 1.0, 0.0);
//
            //glTexCoord2f (1.0, 0.0);
            //glVertex3f (1.0, -1.0, 0.0);
//
            //glTexCoord2f (0.0, 0.0);
            //glVertex3f (-1.0, -1.0, 0.0);
            //glEnd ();
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
