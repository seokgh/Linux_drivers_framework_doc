///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
1. V4L2大致框架分析
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
以usb camera为例：



------------------------------------------------------------------------------------------------------------------------------------------
//App:	open,read,write,ioctl 
------------------------------------------------------------------------------------------------------------------------------------------
	app就没啥了，最终会和 核心层file_operations打交道了。
	app的open,read,write,ioctl...等会调用到file_operations(v4l2_fops)中的那些函数。

------------------------------------------------------------------------------------------------------------------------------------------
//v4l2-dev.c 核心层
------------------------------------------------------------------------------------------------------------------------------------------
1.分配 cdev:	cdev_alloc();
2.设置 cdev:	cdev->ops = v4l2_fops;	//app最终会调用到它里面的open,read,write,ioctl...
3.注册 cdev:	cdev_add();


					//在v4l2-dev.c中写好了的 标准的，统一的 file_operations 结构体 v4l2_fops 结构实体:
					static const struct file_operations v4l2_fops = {
						.owner = THIS_MODULE,
						.read = v4l2_read,
						.write = v4l2_write,
						.open = v4l2_open,
						.get_unmapped_area = v4l2_get_unmapped_area,
						.mmap = v4l2_mmap,
						.unlocked_ioctl = v4l2_ioctl,
					#ifdef CONFIG_COMPAT
						.compat_ioctl = v4l2_compat_ioctl32,
					#endif
						.release = v4l2_release,
						.poll = v4l2_poll,
						.llseek = no_llseek,
					};


					//如 v4l2_read 就是这么实现的：
						struct video_device *vdev = video_devdata(filp); 	//根据次设备号从video_device[]数组中得到video_device结构
							ret = vdev->fops->read(filp, buf, sz, off);	//就是到★了(最底层了)

------------------------------------------------------------------------------------------------------------------------------------------
//uvc_driver.c 最底层
------------------------------------------------------------------------------------------------------------------------------------------
1.分配 video_device：	video_device_alloc
2.设置 video_device：	
3.注册 video_device：	video_register_device



uvc_probe:
	->v4l2_device_register	//它不重要，叫init更合适，因为没"注册"什么。
	->uvc_register_chains
		->uvc_register_terms
			->uvc_register_video	//有俩文件，注意是uvc_driver.c文件，而不是f_uvc.c文件！
				vdev = video_device_alloc();	//1.分配video_device
								//2.设置video_device
				vdev->v4l2_dev = &dev->vdev;
				vdev->fops = &uvc_fops;		//★	const struct v4l2_file_operations *fops;
				vdev->release = uvc_release;
				strlcpy(vdev->name, dev->name, sizeof vdev->name);

				->video_register_device(vdev, VFL_TYPE_GRABBER, UVC_VIDEO_NR);	//3.注册video_device。v4l2-dev.c 核心层





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
2. 具体怎么写 最底层的那个驱动文件？ 可以参考 vivi.c 虚拟视频驱动文件，它刚好作为参考。 会发现它其实和uvc_driver.c流程差不多。
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//vivi.c 虚拟视频驱动

//整个最底层文件该实现的关键点是：
1.分配 video_device：	video_device_alloc
2.设置 video_device：	
3.注册 video_device：	video_register_device


//那现在来分析看看是不是那样实现的：
vivi_init
	->vivi_create_instance
		->v4l2_device_register		//它不重要，因为没"注册"什么,叫init更合适。只是用于初始化一些东西：自旋锁，引用计数
		...
		//1.分配video_device
		->vfd = video_device_alloc();	
		//2.设置video_device
		初始化fops指针等了
		//3.注册video_device
		->ret = video_register_device(vfd, VFL_TYPE_GRABBER, video_nr);

//发现vivi.c虚拟视频驱动文件其实就是实现了上面那3个关键点，跟uvc_driver.c没什么两样。
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






//问1：分析一下"3.注册 video_device"用的 video_register_device 核心层函数到底干了什么？
video_register_device(struct video_device *vdev, int type, int nr)	//根据这个类型type,创建不同的设备节点,次设备号也是不一样的
	-> __video_register_device(vdev, type, nr, 1, vdev->fops->owner);

		vdev->cdev = cdev_alloc();	//

		vdev->cdev->ops = &v4l2_fops;	//它里面肯定会调用到vfd的fops，即调用最底层的的fops了。跟uvc一样了。

		ret = cdev_add(vdev->cdev, MKDEV(VIDEO_MAJOR, vdev->minor), 1);

		video_device[vdev->minor] = vdev;	//放进数组video_device[],以次设备号为下标。

		vdev->ctrl_handler = vdev->v4l2_dev->ctrl_handler;	//

//答：所以不管视频驱动多么复杂，最终的框架是一样的：向上呈现为字符设备了： cdev alloc --> cdev设置(cdev里有file_operations结构体) --> cdev_add。






//问2：分析一下"2.设置 video_device"都设置了什么？ 即 video_device 被alloc了之后，设置了哪些东西？
//答2：设置了如下东西：
	1.	.fops 		= &vivi_fops,
		.ioctl_ops 	= &vivi_ioctl_ops,
		.release	= video_device_release,
		.tvnorms        = V4L2_STD_525_60,
		.current_norm   = V4L2_STD_NTSC_M,

	2. vfd->v4l2_dev = &dev->v4l2_dev;	//有什么作用？？？ v4l2_dev来自v4l2_device_register.

	3. 初始化一个v4l2_ctrl_handler链表，并设置它(添加各种属性)。
		初始化:	v4l2_ctrl_handler_init(hdl, 11);	//11是个数
		设置:	创建各种"v4l2_ctrl属性"，放入链表：用v4l2_ctrl_new_std，v4l2_ctrl_new_custom，如下：	
			设置各种"v4l2_ctrl属性",用于APP的ioctl. 如App通过某条ioctl获取默认亮度值,设置亮度等. 即ioctl能做什么事情，要事先给添加"v4l2_ctrl属性"！

			dev->fmt = &formats[0];
			dev->width = 640;
			dev->height = 480;
			hdl = &dev->ctrl_handler;
			v4l2_ctrl_handler_init(hdl, 11);
			dev->volume 	= v4l2_ctrl_new_std(hdl, &vivi_ctrl_ops, V4L2_CID_AUDIO_VOLUME, 0, 255, 1, 200); //音量
			dev->brightness = v4l2_ctrl_new_std(hdl, &vivi_ctrl_ops, V4L2_CID_BRIGHTNESS, 0, 255, 1, 127);	 //亮度(最小亮度值，最大亮度值，默认值) 
			dev->contrast	= v4l2_ctrl_new_std(hdl, &vivi_ctrl_ops, V4L2_CID_CONTRAST, 0, 255, 1, 16);	
			...
			dev->button 	= v4l2_ctrl_new_custom(hdl, &vivi_ctrl_button, NULL);	//添加一个客户custom的ctrl
			dev->int32 	= v4l2_ctrl_new_custom(hdl, &vivi_ctrl_int32, NULL);
			...
			共11个属性追加。。。


	//1.的具体代码如下：
			*vfd = vivi_template;
			vfd->debug = debug;
			vfd->v4l2_dev = &dev->v4l2_dev;
			set_bit(V4L2_FL_USE_FH_PRIO, &vfd->flags);
								static struct video_device vivi_template = {
									.name		= "vivi",	//名字
									.fops           = &vivi_fops,	//	const struct v4l2_file_operations *fops;
									.ioctl_ops 	= &vivi_ioctl_ops,//
									.release	= video_device_release,

									.tvnorms        = V4L2_STD_525_60,
									.current_norm   = V4L2_STD_NTSC_M,
								};

								static const struct v4l2_file_operations vivi_fops = {
									.owner		= THIS_MODULE,
									.open		= v4l2_fh_open,	//app调用到此
									.release        = vivi_close,	//app调用到此
									.read           = vivi_read,	//app调用到此
									.poll		= vivi_poll,	//app调用到此
									.unlocked_ioctl = video_ioctl2, //app调用到此
									.mmap           = vivi_mmap,	//app调用到此
								};







//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
3. 可以分析 vivi.c 的读/写过程了。  对于一个驱动，若想彻底分析它，就得分析它的open，read，write等等过程
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
分析vivi.c的open，read，write等过程
1. open过程：
	app:	open("/dev/video0",....);
	----------------------------------------------------
	drv:	核心层v4l2-dev.c里的v4l2_fops里的open函数：v4l2_open()


			static int v4l2_open(struct inode *inode, struct file *filp)
			{
				struct video_device *vdev;
				...
				vdev = video_devdata(filp);	//根据次设备号从video_device[]数组中得到video_device结构！
								//那问：video_device结构又是什么时候被放进数组中去的？-在__video_register_device里放的。
								//即，驱动注册时以次设备号为下标加入数组中，App来open时再用次设备号从数组中取出来。
				...
				//取出video_device来之后，就开始调它的各种方法了。
				if (vdev->fops->open) {
					...
					ret = vdev->fops->open(filp);
						  //vivi_fops
						  	//v4l2_fh_open	//实质调用的函数
					...
				}
				...
			}
	open过程中，还设置了一些东西，(比如初始化队列...)暂且忽略...

2. read过程：
	app:	read(....);
	----------------------------------------------------
	drv:	核心层v4l2-dev.c里的v4l2_fops里的read函数：v4l2_read()

		跟open过程一样了：
		根据次设备号从video_device[]数组中得到video_device结构，再调用它的read： ret = vdev->fops->read(filp, buf, sz, off);

3. write过程：
	也是跟open，read过程一样...

4. ioctl过程： 才是较复杂的部分了
	app:	ioctl(....);
	----------------------------------------------------
	drv:	核心层v4l2-dev.c里的v4l2_fops里的unlocked_ioctl函数：v4l2_ioctl()
	
			static long v4l2_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
			{
				struct video_device *vdev = video_devdata(filp);//也先以次设备号从video_device[]数组中得到video_device结构
				...
				ret = vdev->fops->unlocked_ioctl(filp, cmd, arg);//实质上调用的就是video_ioctl2
			}

		//v4l2-ioctl.c 文件
		long video_ioctl2(struct file *file, unsigned int cmd, unsigned long arg)
		{
			//功能就是：把UserSpace来的参数拷贝进来，然后调用__video_do_ioctl函数了。
			return video_usercopy(file, cmd, arg, __video_do_ioctl);
		}
		
		//
		static long __video_do_ioctl(struct file *file, unsigned int cmd, void *arg)
		{
			//1.跟open，read一样了。根据次设备号从video_device[]数组中得到video_device结构
			struct video_device *vfd = video_devdata(file);	
			...
			//2.根据app传入的cmd，来 得到/设置 某些属性！	//问：这些属性又是在哪儿被设置的呢？ 
			switch (cmd) {					//答：在vivi.c里初始化一个v4l2_ctrl_handler链表的时候。
			/* --- capabilities --- */
			case VIDIOC_QUERYCAP:
			{...}
			/* --- priority --- */
			case VIDIOC_G_PRIORITY:
			{...}
			case VIDIOC_XXX:
			{...}
			
		}










//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
4. 最后总结： 怎么写V4L2驱动：
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
1. 分配,设置,注册v4l2_device:	用关键函数 v4l2_device_register() //实际上它并不重要，只是一些辅助作用：如自旋锁，引用计数 等供以后使用而已。此函数最面改写为init比较好一点。
				用此函数得到一个 v4l2_device ?
2. 分配video_device结构体(vfd):	用关键函数 video_device_alloc()
3. 设置video_device结构体(vfd):	
		a. 它里面又有一些机构体:
				vfd->v4l2_dev = 第一步得到的那个v4l2_device;	//要用那些辅助作用的东西
		b. .fops 	= &xxx_fops,	之后，再写xxx_fops里面的open，read，write等函数
		   .ioctl_ops 	= &vivi_ioctl_ops,
 
		c. App可以通过ioctl来 设置/取得 亮度等等信息，
			那么，驱动里谁来 接受/存储/提供 这些信息？ 又是由谁来管理它们？
				属性：驱动里抽象出了个结构体v4l2_ctrl，由v4l2_ctrl来表示。每个ctrl对应一项，如音量，亮度等。 最终加入到链表v4l2_ctrl_handler里。		
				管理：由v4l2_ctrl_handler链表来保存着（见vivi.c初始化部分）。

				1.初始化链表：	v4l2_ctrl_handler_init()
				2.设置链表：	v4l2_ctrl_new_std()	//创建v4l2_ctrl，放入链表。
						v4l2_ctrl_new_custom()	//创建v4l2_ctrl，放入链表。
				3.与vdev关联：	v4l2_dev.ctrl_handler = hdl;	//hdl就是1,2步骤弄出来的。
						vfd->v4l2_dev = &dev->v4l2_dev;	//这样就关联起来了(在vivi.c初始化开头处就能看见)。
			
			这些v4l2_ctrl属性被放入链表之后，怎么被使用的？ 是由驱动的ioctl来引用的：
			v4l2_ctrl_handler的使用过程： 
				同样由app调用ioctl，一路调用下来，到达__video_do_ioctl：
				static long __video_do_ioctl(struct file *file, unsigned int cmd, void *arg)
				{
					//1.跟open，read一样，根据次设备号从video_device[]数组中得到video_device结构
					struct video_device *vfd = video_devdata(file);
					...
					...					
					/* --- controls --- */
					case VIDIOC_QUERYCTRL:
					{
						struct v4l2_queryctrl *p = arg;

						if (vfh && vfh->ctrl_handler)
							ret = v4l2_queryctrl(vfh->ctrl_handler, p);
						else if (vfd->ctrl_handler)	//在哪儿设置上的？ 在video_register_device函数里设置: vdev->ctrl_handler = vdev->v4l2_dev->ctrl_handler;
							//干嘛用的？ 根据id(创建v4l2_ctrl时都有个id)在v4l2_ctrl_handler里找到所对应的v4l2_ctrl，返回它的值。
							ret = v4l2_queryctrl(vfd->ctrl_handler, p);	
						else if (ops->vidioc_queryctrl)
							ret = ops->vidioc_queryctrl(file, fh, p);
						....
					}
				}					
					
			

