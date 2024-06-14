#include <string.h>
#include <debug.h>

/* SRC에서 DST로 SIZE 바이트를 복사합니다. 겹쳐서는 안됩니다.
   DST를 반환합니다. */
void *
memcpy (void *dst_, const void *src_, size_t size) {
	unsigned char *dst = dst_;
	const unsigned char *src = src_;

	ASSERT (dst != NULL || size == 0);
	ASSERT (src != NULL || size == 0);

	while (size-- > 0)
		*dst++ = *src++;

	return dst_;
}

/* SRC에서 DST로 SIZE 바이트를 복사합니다. 겹쳐도 괜찮습니다.
   DST를 반환합니다. */
void *
memmove (void *dst_, const void *src_, size_t size) {
	unsigned char *dst = dst_;
	const unsigned char *src = src_;

	ASSERT (dst != NULL || size == 0);
	ASSERT (src != NULL || size == 0);

	if (dst < src) {
		while (size-- > 0)
			*dst++ = *src++;
	} else {
		dst += size;
		src += size;
		while (size-- > 0)
			*--dst = *--src;
	}

	return dst;
}

/* A와 B에서 SIZE 바이트 동안 다른 첫 번째 바이트를 찾습니다.
   A의 바이트가 더 크면 양수를 반환하고, B의 바이트가 더 크면 음수를 반환하며,
   A와 B가 같으면 0을 반환합니다. */
int
memcmp (const void *a_, const void *b_, size_t size) {
	const unsigned char *a = a_;
	const unsigned char *b = b_;

	ASSERT (a != NULL || size == 0);
	ASSERT (b != NULL || size == 0);

	for (; size-- > 0; a++, b++)
		if (*a != *b)
			return *a > *b ? +1 : -1;
	return 0;
}

/* 문자열 A와 B에서 다른 첫 번째 문자를 찾습니다.
   A의 문자가 더 크면 양수를 반환하고, B의 문자가 더 크면 음수를 반환하며,
   A와 B가 같으면 0을 반환합니다. */
int
strcmp (const char *a_, const char *b_) {
	const unsigned char *a = (const unsigned char *) a_;
	const unsigned char *b = (const unsigned char *) b_;

	ASSERT (a != NULL);
	ASSERT (b != NULL);

	while (*a != '\0' && *a == *b) {
		a++;
		b++;
	}

	return *a < *b ? -1 : *a > *b;
}

/* BLOCK의 처음 SIZE 바이트 내에서 CH의 첫 번째 발생을 찾습니다.
   BLOCK에서 CH가 발생하지 않으면 null 포인터를 반환합니다. */
void *
memchr (const void *block_, int ch_, size_t size) {
	const unsigned char *block = block_;
	unsigned char ch = ch_;

	ASSERT (block != NULL || size == 0);

	for (; size-- > 0; block++)
		if (*block == ch)
			return (void *) block;

	return NULL;
}

/* 문자열에서 C의 첫 번째 발생을 찾고 반환합니다. STRING에 C가 나타나지 않으면 null 포인터를 반환합니다.
   C == '\0' 인 경우 문자열의 끝에 있는 null 종결자를 가리키는 포인터를 반환합니다. */
char *
strchr (const char *string, int c_) {
	char c = c_;

	ASSERT (string);

	for (;;)
		if (*string == c)
			return (char *) string;
		else if (*string == '\0')
			return NULL;
		else
			string++;
}

/* STOP에 포함되지 않은 문자로 구성된 STRING의 초기 하위 문자열의 길이를 반환합니다. */
size_t
strcspn (const char *string, const char *stop) {
	size_t length;

	for (length = 0; string[length] != '\0'; length++)
		if (strchr (stop, string[length]) != NULL)
			break;
	return length;
}

/* STRING에서 STOP에 포함된 첫 번째 문자를 가리키는 포인터를 반환합니다.
   STRING에 STOP에 포함된 문자가 없으면 null 포인터를 반환합니다. */
char *
strpbrk (const char *string, const char *stop) {
	for (; *string != '\0'; string++)
		if (strchr (stop, *string) != NULL)
			return (char *) string;
	return NULL;
}

/* STRING에서 C의 마지막 발생을 가리키는 포인터를 반환합니다.
   STRING에 C가 발생하지 않으면 null 포인터를 반환합니다. */
char *
strrchr (const char *string, int c_) {
	char c = c_;
	const char *p = NULL;

	for (; *string != '\0'; string++)
		if (*string == c)
			p = string;
	return (char *) p;
}

/* STRING의 초기 하위 문자열의 길이를 반환합니다.
   이 하위 문자열은 SKIP에 포함된 문자로만 구성됩니다. */
size_t
strspn (const char *string, const char *skip) {
	size_t length;

	for (length = 0; string[length] != '\0'; length++)
		if (strchr (skip, string[length]) == NULL)
			break;
	return length;
}

/* HAYSTACK 내에서 NEEDLE의 첫 번째 발생을 가리키는 포인터를 반환합니다.
   HAYSTACK 내에 NEEDLE이 없으면 null 포인터를 반환합니다. */
char *
strstr (const char *haystack, const char *needle) {
	size_t haystack_len = strlen (haystack);
	size_t needle_len = strlen (needle);

	if (haystack_len >= needle_len) {
		size_t i;

		for (i = 0; i <= haystack_len - needle_len; i++)
			if (!memcmp (haystack + i, needle, needle_len))
				return (char *) haystack + i;
	}

	return NULL;
}

/* DELIMITERS로 구분된 토큰으로 문자열을 나눕니다.
   이 함수가 처음 호출될 때 S는 토큰화할 문자열이어야 하고,
   이후 호출에서는 null 포인터여야 합니다.
   SAVE_PTR는 토크나이저의 위치를 추적하는 데 사용되는 'char *' 변수의 주소입니다.
   각 호출마다 반환 값은 문자열의 다음 토큰이며, 토큰이 더 이상 없으면 null 포인터를 반환합니다.

   이 함수는 여러 인접한 구분자를 단일 구분자로 처리합니다.
   반환된 토큰의 길이는 0이 아닙니다.
   DELIMITERS는 하나의 문자열 내에서 각 호출마다 변경될 수 있습니다.

   strtok_r()는 문자열 S를 수정하여 구분자를 null 바이트로 변경합니다.
   따라서 S는 수정 가능한 문자열이어야 합니다.
   특히 문자열 리터럴은 C에서 수정 불가능하므로 const로 선언됩니다.

   사용 예:

   char s[] = "  String to  tokenize. ";
   char *token, *save_ptr;

   for (token = strtok_r (s, " ", &save_ptr); token != NULL;
   token = strtok_r (NULL, " ", &save_ptr))
   printf ("'%s'\n", token);

다음과 같은 출력을 생성합니다:

'String'
'to'
'tokenize.'
*/
char *
strtok_r (char *s, const char *delimiters, char **save_ptr) {
	char *token;

	ASSERT (delimiters != NULL);
	ASSERT (save_ptr != NULL);

	/* S가 null이 아닌 경우, S에서 시작합니다.
	   S가 null인 경우, 저장된 위치에서 시작합니다. */
	if (s == NULL)
		s = *save_ptr;
	ASSERT (s != NULL);

	/* 현재 위치에서 DELIMITERS를 건너뜁니다. */
	while (strchr (delimiters, *s) != NULL) {
		/* strchr()는 우리가 null 바이트를 찾고 있다면 항상 null이 아닌 값을 반환합니다.
		   모든 문자열은 끝에 null 바이트를 포함하기 때문입니다. */
		if (*s == '\0') {
			*save_ptr = s;
			return NULL;
		}

		s++;
	}

	/* 문자열 끝까지 DELIMITERS가 아닌 문자를 건너뜁니다. */
	token = s;
	while (strchr (delimiters, *s) == NULL)
		s++;
	if (*s != '\0') {
		*s = '\0';
		*save_ptr = s + 1;
	} else
		*save_ptr = s;
	return token;
}

/* DST의 SIZE 바이트를 VALUE로 설정합니다. */
void *
memset (void *dst_, int value, size_t size) {
	unsigned char *dst = dst_;

	ASSERT (dst != NULL || size == 0);

	while (size-- > 0)
		*dst++ = value;

	return dst_;
}

/* STRING의 길이를 반환합니다. */
size_t
strlen (const char *string) {
	const char *p;

	ASSERT (string);

	for (p = string; *p != '\0'; p++)
		continue;
	return p - string;
}

/* STRING의 길이가 MAXLEN 문자보다 작으면 실제 길이를 반환합니다.
   그렇지 않으면 MAXLEN을 반환합니다. */
size_t
strnlen (const char *string, size_t maxlen) {
	size_t length;

	for (length = 0; string[length] != '\0' && length < maxlen; length++)
		continue;
	return length;
}

/* 문자열 SRC를 DST에 복사합니다. SRC가 SIZE - 1 문자보다 길면,
   SIZE - 1 문자만 복사됩니다. DST에는 항상 null 종결자가 쓰이며,
   SIZE가 0인 경우는 제외됩니다.
   null 종결자를 포함하지 않은 SRC의 길이를 반환합니다.

   strlcpy()는 표준 C 라이브러리에 포함되지 않지만,
   점점 더 인기 있는 확장 기능입니다. 자세한 내용은
   http://www.courtesan.com/todd/papers/strlcpy.html 을 참조하십시오. */
size_t
strlcpy (char *dst, const char *src, size_t size) {
	size_t src_len;

	ASSERT (dst != NULL);
	ASSERT (src != NULL);

	src_len = strlen (src);
	if (size > 0) {
		size_t dst_len = size - 1;
		if (src_len < dst_len)
			dst_len = src_len;
		memcpy (dst, src, dst_len);
		dst[dst_len] = '\0';
	}
	return src_len;
}

/* 문자열 SRC를 DST에 연결합니다. 연결된 문자열은
   SIZE - 1 문자로 제한됩니다. DST에는 항상 null 종결자가 쓰이며,
   SIZE가 0인 경우는 제외됩니다. 충분한 공간이 있다고 가정했을 때
   연결된 문자열의 길이를 반환합니다. null 종결자는 포함하지 않습니다.

   strlcat()는 표준 C 라이브러리에 포함되지 않지만,
   점점 더 인기 있는 확장 기능입니다. 자세한 내용은
   http://www.courtesan.com/todd/papers/strlcpy.html 을 참조하십시오. */
size_t
strlcat (char *dst, const char *src, size_t size) {
	size_t src_len, dst_len;

	ASSERT (dst != NULL);
	ASSERT (src != NULL);

	src_len = strlen (src);
	dst_len = strlen (dst);
	if (size > 0 && dst_len < size) {
		size_t copy_cnt = size - dst_len - 1;
		if (src_len < copy_cnt)
			copy_cnt = src_len;
		memcpy (dst + dst_len, src, copy_cnt);
		dst[dst_len + copy_cnt] = '\0';
	}
	return src_len + dst_len;
}