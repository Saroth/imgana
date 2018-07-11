#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include "library_loader.h"

const char *log_filename = "imgana.log";

LibraryLoader::LibraryLoader()
{
    flag_running = false;
    handler = 0;
    funcs = 0;
    image_data = 0;
    image_data_size = 0;

    if ((log_fd = open(log_filename, O_WRONLY | O_CREAT | O_APPEND)) < 0) {
        log_output(__FILE__, __LINE__, "open log file(%s) failed: %s",
                log_filename, strerror(errno));
        log_fd = -1;
    }
}

LibraryLoader::~LibraryLoader()
{
    delete []image_data;
    image_data = 0;
    image_data_size = 0;
}

int LibraryLoader::log_output(const char *file, size_t line,
        const char *fmt, ...)
{
    char buf[1024];
    size_t len = 0;

    if (file) {
        file = strrchr(file, '\\') ? (strrchr(file, '\\') + 1) :
            strrchr(file, '/') ? (strrchr(file, '/') + 1) : file;
        len = snprintf(buf, sizeof(buf), "%20s:%04ld  ", file, line);
    }

    va_list va;
    va_start(va, fmt);
    len += vsnprintf(buf + len, sizeof(buf) - len, fmt, va);
    va_end(va);

    write(log_fd, buf, len);
    write(log_fd, "\n", 1);

    return len;
}

const char *LibraryLoader::log_file()
{
    return log_filename;
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
    log_output(__FILE__, __LINE__, "load image file: %s.", file_name);
    int err;
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        err = errno;
        log_output(__FILE__, __LINE__, "open failed, return: %d", fd);
        log_output(__FILE__, __LINE__, "error info: [%d]%s",
                err, strerror(err));
        return LIBANA_ERR_OPEN_FAILED;
    }
    int ret;
    do {
        struct stat st;
        if ((ret = fstat(fd, &st)) != 0) {
            err = errno;
            log_output(__FILE__, __LINE__, "fstat failed, return: %d", ret);
            log_output(__FILE__, __LINE__,
                    "error info: [%d]%s", err, strerror(err));
            ret = LIBANA_ERR_FSTAT_FAILED;
            break;
        }
        if (!S_ISREG(st.st_mode)) {
            log_output(__FILE__, __LINE__, "not a regular file!");
            ret = LIBANA_ERR_NOT_REGULAR_FILE;
            break;
        }
        image_data_size = st.st_size;
        log_output(__FILE__, __LINE__, "file size: %d", image_data_size);

        delete []image_data;
        image_data = new unsigned char[image_data_size + 4];
        size_t count = 0;
        while (image_data_size > count) {
            ret = read(fd, &image_data[count], image_data_size - count);
            if (ret < 0) {
                err = errno;
                log_output(__FILE__, __LINE__,
                        "read failed, return: %d", ret);
                log_output(__FILE__, __LINE__,
                        "error info: [%d]%s", err, strerror(err));
                ret = LIBANA_ERR_READ_FILE;
                break;
            }
            else if (ret == 0) {
                log_output(__FILE__, __LINE__,
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
    log_output(__FILE__, __LINE__, "loading library: %s.", LIBRARY_FILE);
    void *h;
    if ((h = dlopen(LIBRARY_FILE, RTLD_LAZY)) == 0) {
        log_output(__FILE__, __LINE__,
                "library load failed: %s.", dlerror());
        return LIBANA_ERR_LOAD_LIBRARY_FAILED;
    }

    func_libana_ioctl ioctl = (func_libana_ioctl)dlsym(h,
            LIBANA_IOCTL_FUNC_NAME);
    char *err = dlerror();
    if (err) {
        log_output(__FILE__, __LINE__,
                "dlsym(%s) error: %s", LIBANA_IOCTL_FUNC_NAME, err);
        dlclose(h);
        return LIBANA_ERR_SYMBOL_IS_NOT_FOUND;
    }
    funcs = ioctl();
    handler = h;
    log_output(__FILE__, __LINE__, "library version: %s", library_version());

    int ret;
    funcs->set_memory_alloc(cb_alloc, cb_free, this);
    funcs->set_debug(cb_output, 0, this);
    funcs->set_mark_point(f_mark_point, p_mark_point);
    funcs->set_mark_line(f_mark_line, p_mark_line);
    if ((ret = funcs->init(&context)) < 0) {
        log_output(__FILE__, __LINE__,
                "library init failed, return:%#x.", -ret);
        return ret;
    }
    log_output(__FILE__, __LINE__, "load successfully.");
    return 0;
}

void LibraryLoader::unload()
{
    if (handler) {
        funcs->free(context);
        log_output(__FILE__, __LINE__, "close library.");
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
int LibraryLoader::cb_output(void *p, const char *file, size_t line,
        const char *str)
{
    return ((LibraryLoader *)p)->log_output(file, line, str);
}

int LibraryLoader::run(int type)
{
    if (!is_loaded()) {
        log_output(__FILE__, __LINE__, "library not loaded.");
        return LIBANA_ERR_LIBRARY_NOT_LOADED;
    }
    if (!image_data) {
        log_output(__FILE__, __LINE__, "image not loaded.");
        return LIBANA_ERR_IMAGE_NOT_LOADED;
    }
    flag_running = true;
    log_output(__FILE__, __LINE__, "==== import image");
    funcs->import_bmp(context, image_data, image_data_size);
    log_output(__FILE__, __LINE__, ">>>> analyze start");
    int ret = funcs->start(context, type);
    log_output(__FILE__, __LINE__, "<<<< analyze finish");
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
        log_output(__FILE__, __LINE__, "analyzer is not running.");
        return;
    }
    log_output(__FILE__, __LINE__, "stop analyzer.");
    funcs->stop(context);
}

bool LibraryLoader::is_busy()
{
    if (!is_loaded()) {
        log_output(__FILE__, __LINE__, "library not loaded.");
        return false;
    }
    return funcs->is_running(context) ? true : false;
}


