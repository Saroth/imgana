#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include "library_loader.h"

LibraryLoader::LibraryLoader()
{
    flag_running = false;
    handler = 0;
    f_debug = 0;
    funcs = 0;
    image_data = 0;
    image_data_size = 0;
}

LibraryLoader::~LibraryLoader()
{
    delete []image_data;
    image_data = 0;
    image_data_size = 0;
}

int LibraryLoader::sdb_out_info(const char *file, size_t line,
        const char *fmt, ...)
{
    char buf[1024];
    va_list va;
    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    file = strrchr(file, '\\') ? (strrchr(file, '\\') + 1) :
        strrchr(file, '/') ? (strrchr(file, '/') + 1) : file;
    if (f_debug) {
        return f_debug(p_debug, file, line, buf);
    }

    return printf("%20s:%04ld  %s\n", file, line, buf);
}



void LibraryLoader::set_debug(func_analyzer_bio_debug f, void *p)
{
    f_debug = f;
    p_debug = p;
}

void LibraryLoader::set_mark_point(func_analyzer_bio_mark_point f, void *p)
{
    f_mark_point = f;
    p_mark_point = p;
}

void LibraryLoader::set_mark_line(func_analyzer_bio_mark_line f, void *p)
{
    f_mark_line = f;
    p_mark_line = p;
}

int LibraryLoader::set_image(const char *file_name)
{
    sdb_out_info(__FILE__, __LINE__, "load image file: %s.", file_name);
    int err;
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        err = errno;
        sdb_out_info(__FILE__, __LINE__, "open failed, return: %d", fd);
        sdb_out_info(__FILE__, __LINE__, "error info: [%d]%s",
                err, strerror(err));
        return LIBANA_ERR_OPEN_FAILED;
    }
    int ret;
    do {
        struct stat st;
        if ((ret = fstat(fd, &st)) != 0) {
            err = errno;
            sdb_out_info(__FILE__, __LINE__, "fstat failed, return: %d", ret);
            sdb_out_info(__FILE__, __LINE__, "error info: [%d]%s",
                    err, strerror(err));
            ret = LIBANA_ERR_FSTAT_FAILED;
            break;
        }
        if (!S_ISREG(st.st_mode)) {
            sdb_out_info(__FILE__, __LINE__, "not a regular file!");
            ret = LIBANA_ERR_NOT_REGULAR_FILE;
            break;
        }
        image_data_size = st.st_size;
        sdb_out_info(__FILE__, __LINE__, "file size: %d", image_data_size);

        delete []image_data;
        image_data = new unsigned char[image_data_size + 4];
        size_t count = 0;
        while (image_data_size > count) {
            ret = read(fd, &image_data[count], image_data_size - count);
            if (ret < 0) {
                err = errno;
                sdb_out_info(__FILE__, __LINE__, "read failed, return: %d",
                        ret);
                sdb_out_info(__FILE__, __LINE__, "error info: [%d]%s",
                        err, strerror(err));
                ret = LIBANA_ERR_READ_FILE;
                break;
            }
            else if (ret == 0) {
                sdb_out_info(__FILE__, __LINE__,
                        "reach the end-of-file(%d < %d), should never happen.",
                        count, image_data_size);
                ret = LIBANA_ERR_BAD_SIZE;
                break;
            }
            count += ret;
        }
        if (ret < 0) {
            break;
        }
    } while (0);
    close(fd);

    return ret;
}

int LibraryLoader::load()
{
    unload();
    sdb_out_info(__FILE__, __LINE__, "loading library: %s.", LIBRARY_FILE);
    void *h;
    if ((h = dlopen(LIBRARY_FILE, RTLD_LAZY)) == 0) {
        sdb_out_info(__FILE__, __LINE__,
                "library load failed: %s.", dlerror());
        return LIBANA_ERR_LOAD_LIBRARY_FAILED;
    }

    func_libana_ioctl ioctl = (func_libana_ioctl)dlsym(h,
            LIBANA_IOCTL_FUNC_NAME);
    char *err = dlerror();
    if (err) {
        sdb_out_info(__FILE__, __LINE__,
                "dlsym(%s) error: %s", LIBANA_IOCTL_FUNC_NAME, err);
        dlclose(h);
        return LIBANA_ERR_SYMBOL_IS_NOT_FOUND;
    }
    funcs = ioctl();
    handler = h;

    sdb_out_info(__FILE__, __LINE__, "library version: %s", library_version());
    sdb_out_info(__FILE__, __LINE__, "load successfully.");
    return 0;
}

void LibraryLoader::unload()
{
    if (handler) {
        sdb_out_info(__FILE__, __LINE__, "close library.");
        dlclose(handler);
        handler = 0;
    }
}

bool LibraryLoader::is_loaded()
{
    return handler ? 1 : 0;
}

const char *LibraryLoader::library_version(void)
{
    if (is_loaded()) {
        return funcs->version_str();
    }
    return "";
}


void *LibraryLoader::cb_alloc(void *p, size_t size)
{
    unsigned char *addr = new unsigned char[size];
    return addr;
}
void LibraryLoader::cb_free(void *p, void *addr)
{
    delete [](unsigned char *)addr;
}

int LibraryLoader::run(int type)
{
    if (!is_loaded()) {
        sdb_out_info(__FILE__, __LINE__, "library not loaded.");
        return LIBANA_ERR_LIBRARY_NOT_LOADED;
    }
    if (!image_data) {
        sdb_out_info(__FILE__, __LINE__, "image not loaded.");
        return LIBANA_ERR_IMAGE_NOT_LOADED;
    }
    flag_running = true;
    funcs->init(&context);
    funcs->set_memory_alloc(&context, cb_alloc, cb_free, this);
    funcs->set_debug(&context, f_debug, p_debug);
    funcs->set_mark_point(&context, f_mark_point, p_mark_point);
    funcs->set_mark_line(&context, f_mark_line, p_mark_line);
    funcs->import_bmp(&context, image_data, image_data_size);
    sdb_out_info(__FILE__, __LINE__, ">>>> analyze start");
    int ret = funcs->start(&context, type);
    sdb_out_info(__FILE__, __LINE__, "<<<< analyze finish");
    funcs->free(&context);
    flag_running = false;

    return ret;
}

bool LibraryLoader::is_running()
{
    return flag_running;
}

void LibraryLoader::stop()
{
    if (!is_running()) {
        sdb_out_info(__FILE__, __LINE__, "analyzer is not running.");
        return;
    }
    sdb_out_info(__FILE__, __LINE__, "stop analyzer.");
    funcs->stop(&context);
}

bool LibraryLoader::is_busy()
{
    if (!is_loaded()) {
        sdb_out_info(__FILE__, __LINE__, "library not loaded.");
        return false;
    }
    return funcs->is_running(&context) ? true : false;
}


