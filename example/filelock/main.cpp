/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-04-03
 * Description: implement the single instance
 */

#include <stdio.h>

#include"lock/file_lock.h"

#include"global/error.h"

int main() {
	TimePass::FileLock file_lock;
	if (false == file_lock.Init("file_mutex")) {
		printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
		return 0;
	}

	if (false == file_lock.TryWLock()) {
		printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
		return 0;
	}

	printf("press any key for end\n");
	getchar();

	if (false == file_lock.UnLock()) {
		printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
		return 0;
	}

	if (false == file_lock.Destroy()) {
		printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
		return 0;
	}

	return 0;
}


