#include <Windows.h>
#include <tchar.h>
#include <string>

#pragma pack(4)

typedef char                s8;
typedef short               s16;
typedef long                s32;
typedef long long           s64;
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned long       u32;
typedef unsigned long long  u64;

typedef struct  
{
    s8      flag[16]; // default value: 531E98204F8542F0
    s32     item_count;
    s64     header_size;
    s64     item_name_size;
} rdb_header_t; // 36 bytes

typedef struct
{
    //s8*     item_name;
    s64     item_content_offset;
    s64     item_content_size;
} item_name_header_t;

void print_help();
inline bool mkdirs(wchar_t* path);
bool write_file(const wchar_t* file_name, void* buf, s64 size);

int _tmain(int argc, wchar_t* argv[])
{
    FILE* fp = NULL;
    rdb_header_t rdb_header;
    item_name_header_t item_header;
    long cur_index = 0;
    wchar_t wc;
    std::wstring file_name;
    void* buf = NULL;
    wchar_t* input_file = NULL;
    wchar_t output_path[MAX_PATH] = {0};

    if (argc != 2)
    {
        print_help();
        return 0;
    }

    input_file = argv[1];
    memcpy(output_path, input_file, wcslen(input_file));
    for (int i=wcslen(output_path)-1; i>0; i--)
    {
        if (output_path[i] == L'.')
        {
            output_path[i] = L'\\';
            output_path[i+1] = 0;
            break;
        }
    }

    if (_waccess(output_path, 0))
    {
        _wrmdir(output_path);
    }

    fp = _wfopen(input_file, L"rb");
    if (!fp)
    {
        printf("Open rdb file fail.\n");
        return 1;
    }

    fread(&rdb_header, 1, sizeof(rdb_header), fp);
    while (rdb_header.item_count--)
    {
        file_name = output_path;
        while (1)
        {
            fread(&wc, 1, sizeof(wc), fp);
            if (wc)
            {
                file_name.push_back(wc);
            }
            else
            {
                fread(&item_header, 1, sizeof(item_header), fp);
                cur_index = ftell(fp);
                fseek(fp, rdb_header.header_size + rdb_header.item_name_size + item_header.item_content_offset, SEEK_SET);
                buf = realloc(buf, item_header.item_content_size);
                fread(buf, 1, item_header.item_content_size, fp);
                fseek(fp, cur_index, SEEK_SET);
                write_file(file_name.c_str(), buf, item_header.item_content_size);
                break;
            }
        }
    }
    fclose(fp);

    if (buf)
    {
        free(buf);
    }

    return 0;
}

inline bool mkdirs(wchar_t* path)
{
    wchar_t temp[MAX_PATH] = {0};
    memcpy(temp, path, wcslen(path)*sizeof(wchar_t));

    for (int i=0; i<wcslen(path); i++)
    {
        if (temp[i] == L'\\')
        {
            temp[i] = 0;
            if (_waccess(temp, 0))
            {
                if (_wmkdir(temp))
                {
                    return false;
                }
            }
            temp[i] = L'\\';
        }
    }

    return true;
}

bool write_file(const wchar_t* file_name, void* buf, s64 size)
{
    FILE* fp = NULL;
    wchar_t dir[MAX_PATH] = {0};

    // create directory
    memcpy(dir, file_name, wcslen(file_name)*sizeof(wchar_t));
    for (int i=wcslen(dir)-1; i>0; i--)
    {
        if (dir[i] == L'\\')
        {
            dir[i+1] = 0;
            break;
        }
    }

    if (_waccess(dir, 0))
    {
        if (!mkdirs(dir))
        {
            printf("mkdir fail.\n");
            return false;
        }
    }

    // write file
    fp = _wfopen(file_name, L"wb");
    if (!fp)
    {
        printf("Open file fail.\n");
        return false;
    }

    if (size != fwrite(buf, 1, size, fp))
    {
        fclose(fp);
        printf("write file fail.\n");
        return false;
    }
    fclose(fp);
    return true;
}

void print_help()
{
    static const char* help_text =
        "Usage:   rdbext.exe [RDB FILE NAME]\n"
        "Example: rdbext.exe res.rdb\n"
        "\n"
        "This command will create a directory, and then extract the resource files to this directory.\n"
        "The directory name is same as input rdb file name.\n";

    printf(help_text);
}