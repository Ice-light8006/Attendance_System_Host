import os

def remove_comments(text: str) -> str:
    """
    删除 C/C++ 风格注释（// 和 /* */），
    保留字符串和字符常量中的内容。
    """
    result = []
    i = 0
    n = len(text)
    state = "NORMAL"  # NORMAL, IN_SINGLE_QUOTE, IN_DOUBLE_QUOTE, IN_LINE_COMMENT, IN_BLOCK_COMMENT

    while i < n:
        ch = text[i]

        if state == "NORMAL":
            if ch == '/' and i + 1 < n:
                next_ch = text[i + 1]
                if next_ch == '/':
                    state = "IN_LINE_COMMENT"
                    i += 2  # 跳过 //，不输出任何内容
                    continue
                elif next_ch == '*':
                    state = "IN_BLOCK_COMMENT"
                    i += 2  # 跳过 /*
                    continue
            # 普通字符或非注释的 '/'
            if ch == '"':
                state = "IN_DOUBLE_QUOTE"
                result.append(ch)
            elif ch == "'":
                state = "IN_SINGLE_QUOTE"
                result.append(ch)
            else:
                result.append(ch)
            i += 1

        elif state == "IN_SINGLE_QUOTE":
            # 字符常量内，直到遇到未转义的 '
            result.append(ch)
            i += 1
            if ch == '\\' and i < n:
                # 转义字符，下一个字符原样保留
                result.append(text[i])
                i += 1
            elif ch == "'":
                state = "NORMAL"

        elif state == "IN_DOUBLE_QUOTE":
            # 字符串内，直到遇到未转义的 "
            result.append(ch)
            i += 1
            if ch == '\\' and i < n:
                result.append(text[i])
                i += 1
            elif ch == '"':
                state = "NORMAL"

        elif state == "IN_LINE_COMMENT":
            # 单行注释，跳过直到换行符
            if ch == '\n':
                result.append(ch)   # 保留换行符，维持行号
                state = "NORMAL"
            i += 1

        elif state == "IN_BLOCK_COMMENT":
            # 块注释，跳过直到 */
            if ch == '*' and i + 1 < n and text[i + 1] == '/':
                i += 2  # 跳过 */
                state = "NORMAL"
            else:
                i += 1

    return ''.join(result)


# 当前目录（脚本所在目录）
current_dir = os.path.dirname(os.path.abspath(__file__))

# 输出文件名
output_file = os.path.join(current_dir, "all_code.txt")

# 支持的文件类型
file_extensions = ['.c', '.cpp', '.h']

with open(output_file, 'w', encoding='utf-8') as out_file:
    # 递归遍历当前目录及其所有子目录
    for root, dirs, files in os.walk(current_dir):
        for file_name in files:
            if any(file_name.endswith(ext) for ext in file_extensions):
                file_path = os.path.join(root, file_name)
                # 计算相对路径，便于区分不同目录下的同名文件
                rel_path = os.path.relpath(file_path, current_dir)
                out_file.write(f"这是{rel_path}\n")

                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        # 删除注释
                        cleaned_content = remove_comments(content)
                        out_file.write(cleaned_content + "\n\n")
                except Exception as e:
                    out_file.write(f"无法读取文件: {e}\n\n")

print(f"整理完成（已删除注释），输出文件：{output_file}")