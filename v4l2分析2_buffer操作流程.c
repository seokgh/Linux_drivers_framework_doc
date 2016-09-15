一. V4L2框架: video for linux version 2


虚拟视频驱动vivi.c分析:
1．分配video_device
2．设置
3．注册：video_register_device

vivi_init
    vivi_create_instance
        v4l2_device_register   // 不是主要, 只是用于初始化一些东西，比如自旋锁、引用计数
        video_device_alloc
        // 设置
          1. vfd:
            .fops           = &vivi_fops,
            .ioctl_ops 	= &vivi_ioctl_ops,
            .release	= video_device_release,
          2.
            vfd->v4l2_dev = &dev->v4l2_dev;
          3. 设置"ctrl属性"(用于APP的ioctl)：
            	v4l2_ctrl_handler_init(hdl, 11);
            	dev->volume = v4l2_ctrl_new_std(hdl, &vivi_ctrl_ops,
            			V4L2_CID_AUDIO_VOLUME, 0, 255, 1, 200);
            	dev->brightness = v4l2_ctrl_new_std(hdl, &vivi_ctrl_ops,
            			V4L2_CID_BRIGHTNESS, 0, 255, 1, 127);
            	dev->contrast = v4l2_ctrl_new_std(hdl, &vivi_ctrl_ops,
            			V4L2_CID_CONTRAST, 0, 255, 1, 16);                        
        video_register_device(video_device, type:VFL_TYPE_GRABBER, nr)
            __video_register_device
                vdev->cdev = cdev_alloc();
                vdev->cdev->ops = &v4l2_fops;
                cdev_add
                
                video_device[vdev->minor] = vdev;

        		if (vdev->ctrl_handler == NULL)
        			vdev->ctrl_handler = vdev->v4l2_dev->ctrl_handler;
                
                

分析vivi.c的open,read,write,ioctl过程
1. open
app:     open("/dev/video0",....)
---------------------------------------------------
drv:     v4l2_fops.v4l2_open
            vdev = video_devdata(filp);  // 根据次设备号从数组中得到video_device
            ret = vdev->fops->open(filp);
                        vivi_ioctl_ops.open
                            v4l2_fh_open

2. read
app:    read ....
---------------------------------------------------
drv:    v4l2_fops.v4l2_read
            struct video_device *vdev = video_devdata(filp);
            ret = vdev->fops->read(filp, buf, sz, off);

3. ioctl
app:   ioctl
----------------------------------------------------
drv:   v4l2_fops.unlocked_ioctl
            v4l2_ioctl
                struct video_device *vdev = video_devdata(filp);
                ret = vdev->fops->unlocked_ioctl(filp, cmd, arg);
                            video_ioctl2
                                video_usercopy(file, cmd, arg, __video_do_ioctl);
                                    __video_do_ioctl
                                        struct video_device *vfd = video_devdata(file);
                                        根据APP传入的cmd来获得、设置"某些属性"

v4l2_ctrl_handler的使用过程:
    __video_do_ioctl
        struct video_device *vfd = video_devdata(file);

    	case VIDIOC_QUERYCTRL:
    	{
    		struct v4l2_queryctrl *p = arg;
    
    		if (vfh && vfh->ctrl_handler)
    			ret = v4l2_queryctrl(vfh->ctrl_handler, p);
    		else if (vfd->ctrl_handler)  // 在哪设置？在video_register_device
    			ret = v4l2_queryctrl(vfd->ctrl_handler, p);
    			            // 根据ID在ctrl_handler里找到v4l2_ctrl，返回它的值
                                                

二、测试虚拟驱动vivi
准备工作：安装xawtv
sudo apt-get install xawtv

源码xawtv-3.95.tar.gz
http://www.kraxel.org/releases/xawtv/

在这个网站创建新的sources.list
http://repogen.simplylinux.ch/
1. 选择国家
2．选择相邻的ubuntu版本
3. 选择"Ubuntu Branches"
4. 生成sources.list
5. 把得到内容替换到/etc/apt/sources.list
6. sudo apt-get update
   sudo apt-get install xawtv

测试USB摄像头：
1．让VMWAER处于前台，接上USB摄像头，可以看到生成了/dev/video0
2．执行 xawtv 即可看到图像

测试虚拟摄像头vivi：
1. 确定ubuntu的内核版本
uname -a
Linux book-desktop 2.6.31-14-generic #48-Ubuntu SMP Fri Oct 16 14:04:26 UTC 2009 i686 GNU/Linux
2. 去www.kernel.org下载同版本的内核
   解压后把drivers/media/video目录取出
   修改它的Makefile为：

KERN_DIR = /usr/src/linux-headers-2.6.31-14-generic

all:
        make -C $(KERN_DIR) M=`pwd` modules 

clean:
        make -C $(KERN_DIR) M=`pwd` modules clean
        rm -rf modules.order

obj-m   += vivi.o
obj-m   += videobuf-core.o
obj-m   += videobuf-vmalloc.o
obj-m   += v4l2-common.o
   
3. make
4. sudo modprobe vivi  
   sudo rmmod vivi
   sudo insmod ./vivi.ko

5. ls /dev/video*
6. xawtv -c /dev/videoX


//
3. ioctl(4, VIDIOC_G_FMT
4. for()
        ioctl(4, VIDIOC_ENUM_FMT
5. ioctl(4, VIDIOC_QUERYCAP    // 列举性能
6. ioctl(4, VIDIOC_G_INPUT     // 获得当前使用输入源
7. ioctl(4, VIDIOC_ENUMINPUT   // 列举输入源
8. ioctl(4, VIDIOC_QUERYCTRL   // 查询属性,比如亮度、对比度
9. ioctl(4, VIDIOC_QUERYCAP
10. ioctl(4, VIDIOC_ENUMINPUT













三、根据虚拟驱动vivi的使用过程彻底分析摄像头驱动
// 1~7都是在v4l2_open里调用
1. open
2. ioctl(4, VIDIOC_QUERYCAP



// 3~7 都是在get_device_capabilities里调用
3. for()
        ioctl(4, VIDIOC_ENUMINPUT    // 列举输入源,VIDIOC_ENUMINPUT/VIDIOC_G_INPUT/VIDIOC_S_INPUT不是必需的
4. for()
        ioctl(4, VIDIOC_ENUMSTD      // 列举标准(制式), 不是必需的
5. for()        
        ioctl(4, VIDIOC_ENUM_FMT     // 列举格式

6. ioctl(4, VIDIOC_G_PARM
7. for()
        ioctl(4, VIDIOC_QUERYCTRL    // 查询属性(比如说亮度值最小值、最大值、默认值)



// 8~10都是通过v4l2_read_attr来调用的        
8. ioctl(4, VIDIOC_G_STD             // 获得当前使用的标准(制式), 不是必需的
9. ioctl(4, VIDIOC_G_INPUT 
10. ioctl(4, VIDIOC_G_CTRL           // 获得当前属性, 比如亮度是多少



11. ioctl(4, VIDIOC_TRY_FMT          // 试试能否支持某种格式
12. ioctl(4, VIDIOC_S_FMT            // 设置摄像头使用某种格式


// 13~16在v4l2_start_streaming
13. ioctl(4, VIDIOC_REQBUFS          // 请求系统分配缓冲区
14. for()
        ioctl(4, VIDIOC_QUERYBUF     // 查询所分配的缓冲区
        mmap        
15. for ()
        ioctl(4, VIDIOC_QBUF         // 把所有的缓冲区都放入队列        
16. ioctl(4, VIDIOC_STREAMON         // 启动摄像头



// 17里都是通过v4l2_write_attr来调用的
17. for ()
        ioctl(4, VIDIOC_S_CTRL           // 设置属性
    ioctl(4, VIDIOC_S_INPUT              // 设置输入源
    ioctl(4, VIDIOC_S_STD                // 设置标准(制式), 不是必需的

// v4l2_nextframe > v4l2_waiton    
18. v4l2_queue_all
    v4l2_waiton    
        for ()
        {
            select(5, [4], NULL, NULL, {5, 0})      = 1 (in [4], left {4, 985979})
            ioctl(4, VIDIOC_DQBUF                // de-queue, 把缓冲区从队列中取出
            // 处理, 之前已经通过mmap获得了缓冲区的地址, 就可以直接访问数据        
            ioctl(4, VIDIOC_QBUF                 // 把缓冲区放入队列
        }








xawtv的几大函数：
1. v4l2_open
2. v4l2_read_attr/v4l2_write_attr
3. v4l2_start_streaming
4. v4l2_nextframe/v4l2_waiton




//精简
摄像头驱动程序必需的 11 个ioctl:
    // 表示它是一个摄像头设备
	.vidioc_querycap      = vidioc_querycap,

    /* 用于列举、获得、测试、设置摄像头的数据的格式 */
	.vidioc_enum_fmt_vid_cap  = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap     = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap   = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap     = vidioc_s_fmt_vid_cap,

    /* 缓冲区操作: 申请/查询/放入队列/取出队列 */
	.vidioc_reqbufs       = vidioc_reqbufs,
	.vidioc_querybuf      = vidioc_querybuf,
	.vidioc_qbuf          = vidioc_qbuf,
	.vidioc_dqbuf         = vidioc_dqbuf,

	// 启动/停止
	.vidioc_streamon      = vidioc_streamon,
	.vidioc_streamoff     = vidioc_streamoff,	




////////////////////////////////////////////////////////////////////////////
继续分析数据的获取过程：
////////////////////////////////////////////////////////////////////////////
1. 请求分配缓冲区: ioctl(4, VIDIOC_REQBUFS          // 请求系统分配缓冲区
                        videobuf_reqbufs(队列, v4l2_requestbuffers) // 队列在open函数用videobuf_queue_vmalloc_init初始化
                        // 注意：这个IOCTL只是分配缓冲区的头部信息，真正的缓存还没有分配呢！

2. 查询映射缓冲区:
	ioctl(4, VIDIOC_QUERYBUF         // 查询所分配的缓冲区
		videobuf_querybuf        // 获得缓冲区的数据格式、大小、每一行长度、高度

	mmap(参数里有"大小")   // 在这里才分配缓存！
		v4l2_mmap
		    vivi_mmap
		        videobuf_mmap_mapper
		            videobuf-vmalloc.c里的__videobuf_mmap_mapper
		                    mem->vmalloc = vmalloc_user(pages);   // 在这里才给缓冲区分配空间！！！

3. 把缓冲区放入队列:
	ioctl(4, VIDIOC_QBUF             // 把缓冲区放入队列        
	    videobuf_qbuf
		q->ops->buf_prepare(q, buf, field);  // 调用驱动程序提供的函数做些预处理
		list_add_tail(&buf->stream, &q->stream);  // 把缓冲区放入队列的尾部
		q->ops->buf_queue(q, buf);           // 调用驱动程序提供的"入队列函数"
        

4. 启动摄像头
ioctl(4, VIDIOC_STREAMON
    videobuf_streamon
        q->streaming = 1;
        

5. 用select查询是否有数据
          // 驱动程序里必定有: 产生数据、唤醒进程 的代码！
          v4l2_poll
                vdev->fops->poll
                    vivi_poll   
                        videobuf_poll_stream
                            // 从队列的头部获得缓冲区！
                			buf = list_entry(q->stream.next, struct videobuf_buffer, stream);
                            // 如果没有数据则休眠！
                			poll_wait(file, &buf->done, wait);

    谁来产生数据、谁来唤醒它？
    内核线程 vivi_thread 每30MS执行一次，它调用：
	    vivi_thread_tick
		vivi_fillbuff(fh, buf);  // 构造数据! 真实的摄像头的话，用硬件产生数据。这里只是虚拟的设备，所以用内核线程产生数据。
		wake_up(&buf->vb.done);  // 唤醒进程!
          


6. 有数据后从队列里取出缓冲区
// 有那么多缓冲区，APP如何知道哪一个缓冲区有数据？调用VIDIOC_DQBUF
ioctl(4, VIDIOC_DQBUF 
    vidioc_dqbuf   
        // 在队列里获得有数据的缓冲区
        retval = stream_next_buffer(q, &buf, nonblocking);
        
        // 把它从队列中删掉
        list_del(&buf->stream);
        
        // 把这个缓冲区的状态返回给APP
        videobuf_status(q, b, buf, q->type);


7. 应用程序根据VIDIOC_DQBUF所得到缓冲区状态，知道是哪一个缓冲区有数据，就去读对应的地址(该地址来自前面的mmap)






怎么写摄像头驱动程序:
1. 分配video_device:video_device_alloc
2. 设置
   .fops
   .ioctl_ops (里面需要设置11项)
   如果要用内核提供的缓冲区操作函数，还需要构造一个 videobuf_queue_ops
3. 注册: video_register_device

四、自己写一个虚拟摄像头驱动






                                                

    
[585955.047776] usb 1-1: new high speed USB device using ehci_hcd and address 3
[585955.269142] usb 1-1: configuration #1 chosen from 1 choice
[585955.379843] uvcvideo: Found UVC 1.00 device USB2.0 Camera (1e4e:0102)
[585955.440427] uvcvideo: UVC non compliance - GET_DEF(PROBE) not supported. Enabling workaround.
[585955.554594] input: USB2.0 Camera as /devices/pci0000:00/0000:00:11.0/0000:02:02.0/usb1/1-1/1-1:1.0/input/input5

LD_PRELOAD=/usr/lib/libv4l/v4l1compat.so camorama

sudo apt-get install cheese

多种LINUX下的摄像头工具
https://help.ubuntu.com/community/Webcam

http://blog.csdn.net/hongtao_liu/article/details/5867351
http://blog.csdn.net/hongtao_liu/article/details/5894089
基于V4L2的视频驱动开发

概念：
视频制式 : NTSC PAL V4L2_STD_NTSC V4L2_STD_PAL
视频格式 ：RGB YCbCr 420,422 V4L2_PIX_FMT_UYVY



 
[视频技术手册]中文第5版
http://ishare.iask.sina.com.cn/f/21425721.html
http://ishare.iask.sina.com.cn/f/21425722.html


http://www.360doc.com/content/08/0926/11/14148_1678949.shtml

数字视频的基本概念
http://hi.baidu.com/fengbit/item/78a161375067f1c42f8ec24e

USB协议
http://www.usb.org/developers/devclass_docs

USB Video Class Specification 笔记 
http://blog.csdn.net/chinaunixj/article/details/7394315


基于嵌入式Linux的视频采集系统---UVC驱动模型介绍
http://blog.csdn.net/chinaunixj/article/details/7439870


Class-specific VC Interface Descriptor
e:\kernel_projects\linux-3.4.2\linux-3.4.2\include\linux\usb\Video.h
#define DECLARE_UVC_HEADER_DESCRIPTOR(n)		\
struct UVC_HEADER_DESCRIPTOR(n) {			\
	__u8  bLength;					\
	__u8  bDescriptorType;				\
	__u8  bDescriptorSubType;			\
	__u16 bcdUVC;					\
	__u16 wTotalLength;				\
	__u32 dwClockFrequency;				\
	__u8  bInCollection;				\
	__u8  baInterfaceNr[n];				\
} __attribute__ ((packed))


Input Terminal Descriptor (Camera)


驱动框架分析：
Structure of a driver
---------------------

All drivers have the following structure:

1) A struct for each device instance containing the device state.

2) A way of initializing and commanding sub-devices (if any).

3) Creating V4L2 device nodes (/dev/videoX, /dev/vbiX and /dev/radioX)
   and keeping track of device-node specific data.

4) Filehandle-specific structs containing per-filehandle data;

5) video buffer handling.

This is a rough schematic of how it all relates:

    device instances            : v4l2_device
      |
      +-sub-device instances    : v4l2_subdev
      |
      \-V4L2 device nodes       : video_device, stores V4L2 device node data, this will create the character device
	  |
	  \-filehandle instances    : v4l2_fh

The framework closely resembles the driver structure: it has a v4l2_device
struct for the device instance data, a v4l2_subdev struct to refer to
sub-device instances, the video_device struct stores V4L2 device node data
and in the future a v4l2_fh struct will keep track of filehandle instances
(this is not yet implemented).

The V4L2 framework also optionally integrates with the media framework. If a
driver sets the struct v4l2_device mdev field, sub-devices and video nodes
will automatically appear in the media framework as entities.



console/fs.h:2:20: error: FSlib.h: No such file or directory

No package 'fontsproto' found

error: asm/page.h: No such file or directory

把asm/page.h改为sys/user.h

sudo apt-get install libxaw7-dev


# libraries
LDLIBS  := -lFS

./configure --x-includes=/usr/local/include/X11/fonts
