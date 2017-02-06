# NCD_YGFXY
>项目名称：荧光分析仪软件<br>
> 软件作者：张雄

## Latest release<br>
>版本号：`V1.0.3` <br>
>编译版本：`Build17012201`<br>
>版本发布时间：`2017年1月22日16:42:50`<br>
>版本发布人：`张雄`<br>
>版本说明：`内部测试版本，经过大量测试已趋于稳定，但是仍有bug待发现`
<br>


## Historical release

>版本号： <font color=red>V1.0.3</font><br>
>编译版本：<font color=#DC641E>Build17012201</font><br>
>版本发布时间：<font color=#1E94DC>2017年1月22日16:42:50</font><br>
>版本发布人：<font color=#1EDC47>张雄</font><br>
>版本说明：<font color=#DCDA1E>内部测试版本，经过大量测试已趋于稳定，但是仍有bug待发现</font><br>
<br>


>版本号：<font color=red>V1.0.3</font><br>
>编译版本：<font color=#DC641E>Build17012301</font><br>
>版本发布时间：<font color=#1E94DC>2017年1月23日08:50:07</font><br>
>版本发布人：<font color=#1EDC47>张雄</font><br>
>版本说明：<font color=#DCDA1E>内部测试版本，修复bug</font><br>
> 详细说明：<br>

* ##### bug1： 修复在卡槽有卡的情况下，进入排队界面会立即新建测试
  * ###### bug现象
    * 从样品编号界面返回排队界面，本意是取消当前测试，但是若卡槽有卡，则会立即新建测试，再次跳会样品输入界面。
    * 预读卡界面，如果二维码读取失败等错误时，点击取消测试，如果有其他卡在排队，会返回排队界面，本意是取消当前测试，但是若卡槽有卡，则会立即新建测试，再次跳会样品输入界面。
  * ###### bug解决方案
    1. 进入排队界面立即禁止插卡自动创建功能（包含新建排队界面和恢复排队界面）<br>
            timer_set(&(S_PaiDuiPageBuffer->timer0), 65535);
    2. 如果自动创建功能处于禁止状态，且电机处于最大行程，且卡槽无卡，则激活插卡自动创建测试功能
            if((S_PaiDuiPageBuffer->timer0.interval == 65535) && (MaxLocation == GetGB_MotorLocation()) && (!CardPinIn))
    			{
    				timer_set(&(S_PaiDuiPageBuffer->timer0), 1);
    			}
    3. 如果检测时间到,则检测是否空闲，且插卡了，如果是则新建测试，新建失败则禁用自动创建功能<br>
            if(TimeOut == timer_expired(&(S_PaiDuiPageBuffer->timer0)))
			{
				if((NULL == GetCurrentTestItem()) && (CardPinIn))
				{
                  timer_set(&(S_PaiDuiPageBuffer->timer0), 65535);
				}
				timer_restart(&(S_PaiDuiPageBuffer->timer0));
            }

>版本号：<font color=red>V1.0.3</font><br>
>编译版本：<font color=#DC641E>Build17020601</font><br>
>版本发布时间：<font color=#1E94DC>2017年2月6日16:23:42</font><br>
>版本发布人：<font color=#1EDC47>张雄</font><br>
>版本说明：<font color=#DCDA1E>优化数据保存，减少IO次数</font><br>
> 详细说明：<br>

* ##### 优化1： 以前的数据上传每次需读取SD卡，检查是否有新数据等待上传，现在将是否有新数据的内容放在系统设置数据中，保存在内存中，上传数据时只需检查内存，有数据等待上传才读取SD卡，无数据则不读取
