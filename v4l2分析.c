//V4L2大致框架分析

以usb camera为例：

------------------------------------------------------------------------------------------------------------------------------------------
//uvc_driver.c 最底层
------------------------------------------------------------------------------------------------------------------------------------------
uvc_probe:
	->v4l2_device_register	//叫init更合适，因为没"注册"什么。
	->uvc_register_chains
		->uvc_register_terms
			->uvc_register_video	//有俩文件，注意是uvc_driver.c文件，而不是f_uvc.c文件！
				vdev = video_device_alloc();	//

				vdev->v4l2_dev = &dev->vdev;
				vdev->fops = &uvc_fops;		//★
				vdev->release = uvc_release;
				strlcpy(vdev->name, dev->name, sizeof vdev->name);

				->video_register_device(vdev, VFL_TYPE_GRABBER, UVC_VIDEO_NR);		//v4l2-dev.c 核心层


------------------------------------------------------------------------------------------------------------------------------------------
//v4l2-dev.c 核心层
------------------------------------------------------------------------------------------------------------------------------------------
分配 cdev:	cdev_alloc();
设置 cdev:	cdev->ops = v4l2_fops;	//app最终会调用到它里面的open,read,write,ioctl...
注册 cdev:	cdev_add();
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
						struct video_device *vdev = video_devdata(filp);
						if (video_is_registered(vdev))
							ret = vdev->fops->read(filp, buf, sz, off);	//就是到★了

------------------------------------------------------------------------------------------------------------------------------------------
//App:	open,read,write,ioctl 
------------------------------------------------------------------------------------------------------------------------------------------
	app就没啥了，最终会和 核心层 打交道了。



//至于，怎么写 最底层的那个驱动文件？ 可以参考 vivi.c 文件了。


