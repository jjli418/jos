#include <inc/x86.h>
#include <inc/pmap.h>

void
notbusy(void)
{
        while(inb(0x1F7) & 0x80);       // wait for disk not busy
}


void
readsect(u_char *dst, u_int count, u_int offset)
{
        notbusy();

        outb(0x1F2, count);
        outb(0x1F3, offset);
        outb(0x1F4, offset>>8);
        outb(0x1F5, offset>>16);
        outb(0x1F6, (offset>>24)|0xE0);
        outb(0x1F7, 0x20);      // cmd 0x20 - read sectors

        notbusy();

        insl(0x1F0, dst, count*512/4);
}

// read count bytes at offset from kernel into addr dst
// might copy more than asked
/*
void
readseg_kern(u_int va, u_int count, u_int offset)
{
        u_int i;


	count += (offset & 511); 

        // translate from bytes to sectors
        offset /= 512;

	// count += (offset & 511);  fucking order, offset should not change for this computation

	// read more bytes
        count = (count+511)/512;

	count++;

        // kernel starts at sector 1
        offset++;

        // if this is too slow, we could read lots of sectors at a time.
        // we'd write more to memory than asked, but it doesn't matter --
        // we load in increasing order.
        for(i=0; i<count; i++){
                readsect((u_char*)va, 1, offset+i);
                va += 512;
        }
}
*/


void
readseg_kern(u_int va, u_int count, u_int offset)
{
        u_int i, j;

	char* sect = (char*)(KERNBASE+0x10000);

	for(i = 0; i < count; i++)
	{
		*((u_char*)va + i) = *(sect + offset + i);
	}

	/*

        // round down to sector boundary; offset will round later
        i = va&511;
        count += i;
        va -= i;

        // translate from bytes to sectors
        offset /= 512;
        count = (count+511)/512;

        // kernel starts at sector 1
        //offset++;

        // if this is too slow, we could read lots of sectors at a time.
        // we'd write more to memory than asked, but it doesn't matter --
        // we load in increasing order.
        for(i=0; i<count; i++){

                for(j = 0; j < 512; j++)
                {
                        *((u_char*)va + j) = *((sect + offset*512 + i*512) + j);
                }

                va += 512;
        }
	*/
}

