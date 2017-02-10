# NCD_YGFXY
>项目名称：荧光分析仪软件<br>
> 软件作者：张雄<br>
> 联系方式：565170595 13035142826<br>

## Latest release<br>
>版本号：<font color=red>V1.0.3</font><br>
>编译版本：<font color=#DC641E>Build17021001</font><br>
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
    2. 如果自动创建功能处于禁止状态，且电机处于最大行程，且卡槽无卡，则激活插卡自动创建测试功能
    3. 如果检测时间到,则检测是否空闲，且插卡了，如果是则新建测试，新建失败则禁用自动创建功能<br>

>版本号：<font color=red>V1.0.3</font><br>
>编译版本：<font color=#DC641E>Build17020601</font><br>
>版本发布时间：<font color=#1E94DC>2017年2月6日16:23:42</font><br>
>版本发布人：<font color=#1EDC47>张雄</font><br>
>版本说明：<font color=#DCDA1E>优化数据保存，减少IO次数</font><br>
> 详细说明：<br>

* ##### 优化1： 以前的数据上传每次需读取SD卡，检查是否有新数据等待上传，现在将是否有新数据的内容放在系统设置数据中，保存在内存中，上传数据时只需检查内存，有数据等待上传才读取SD卡，无数据则不读取

>版本号：<font color=red>V1.0.3</font><br>
>编译版本：<font color=#DC641E>Build17020901</font><br>
>版本发布时间：<font color=#1E94DC>2017年2月9日09:35:18</font><br>
>版本发布人：<font color=#1EDC47>张雄</font><br>
>版本说明：<font color=#DCDA1E>增加检验二维码，校准LED亮度，校准测试结果功能</font><br>
> 详细说明：<br>

* ##### 优化1： 检验二维码功能，密码333333
* ##### 优化2： 校准LED亮度功能，密码444444
* ##### 优化3： 校准测试结果功能，密码111111
* ##### 遗留问题：上传数据与设置wifi会竞争串口，导致wifi有时候设置失败

>版本号：<font color=red>V1.0.3</font><br>
>编译版本：<font color=#DC641E>Build17021001</font><br>
>版本发布时间：<font color=#1E94DC>2017年2月10日08:55:32</font><br>
>版本发布人：<font color=#1EDC47>张雄</font><br>
>版本说明：<font color=#DCDA1E>界面优化</font><br>
> 详细说明：<br>

* ##### 优化1： 厂家设置更改为关于，长按关于，输入密码进入各个隐藏功能
* ##### 优化2： 与wifi的at通信，增加3次错误重试机制，3次都失败在返回失败指令
