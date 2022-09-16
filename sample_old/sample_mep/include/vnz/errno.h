#ifndef _VENEZIA_ERRNO_H
#define _VENEZIA_ERRNO_H

/**
 * @j 実行許可がない @ej
 * @e Operation is not permitted @ee
 */
#define SCE_ERROR_ERRNO_EPERM					-2147418111	/* 0x80010001 */

/**
 * @j ファイルがない @ej
 * @e Associated file or directory does not exist @ee
 */
#define SCE_ERROR_ERRNO_ENOENT					-2147418110	/* 0x80010002 */

/**
 * @j 該当するプロセスが存在しない @ej
 * @e The relevant process does not exist @ee
 */
#define SCE_ERROR_ERRNO_ESRCH					-2147418109	/* 0x80010003 */

/**
 * @j 関数呼び出しが割り込まれた @ej
 * @e The function call was interrupted @ee
 */
#define SCE_ERROR_ERRNO_EINTR					-2147418108	/* 0x80010004 */

/**
 * @j 入出力エラー @ej
 * @e Input/output error @ee
 */
#define SCE_ERROR_ERRNO_EIO						-2147418107	/* 0x80010005 */

/**
 * @j 該当するデバイスまたはアドレスはない @ej
 * @e The relevant device or address was not found @ee
 */
#define SCE_ERROR_ERRNO_ENXIO					-2147418106	/* 0x80010006 */

/**
 * @j 引数リストが長い @ej
 * @e Argument list is too long @ee
 */
#define SCE_ERROR_ERRNO_E2BIG					-2147418105	/* 0x80010007 */

/**
 * @j 実行形式エラー @ej
 * @e Execution format error @ee
 */
#define SCE_ERROR_ERRNO_ENOEXEC					-2147418104	/* 0x80010008 */

/**
 * @j ディスクリプタが不正 @ej
 * @e Invalid file descriptor @ee
 */
#define SCE_ERROR_ERRNO_EBADF					-2147418103	/* 0x80010009 */

/**
 * @j 子プロセスがない @ej
 * @e No child process was found @ee
 */
#define SCE_ERROR_ERRNO_ECHILD					-2147418102	/* 0x8001000A */

/**
 * @j リソースが一時的に使用できない @ej
 * @e Resource is temporarily unavailable @ee
 */
#define SCE_ERROR_ERRNO_EAGAIN					-2147418101	/* 0x8001000B */

/**
 * @j 必要なメモリが確保できない @ej
 * @e Not enough memory @ee
 */
#define SCE_ERROR_ERRNO_ENOMEM					-2147418100	/* 0x8001000C */

/**
 * @j アクセス権限がない @ej
 * @e No file access permission @ee
 */
#define SCE_ERROR_ERRNO_EACCES					-2147418099	/* 0x8001000D */

/**
 * @j アドレスが不正 @ej
 * @e Invalid address @ee
 */
#define SCE_ERROR_ERRNO_EFAULT					-2147418098	/* 0x8001000E */

/**
 * @j ブロックデバイスではありません @ej
 * @e Not a block device @ee
 */
#define SCE_ERROR_ERRNO_ENOTBLK					-2147418097	/* 0x8001000F */

/**
 * @j リソースを使用中 @ej
 * @e Mount or device is busy @ee
 */
#define SCE_ERROR_ERRNO_EBUSY					-2147418096	/* 0x80010010 */

/**
 * @j ファイルがある @ej
 * @e File exists @ee
 */
#define SCE_ERROR_ERRNO_EEXIST					-2147418095	/* 0x80010011 */

/**
 * @j 複数デバイスを指定 @ej
 * @e Cross-device link @ee
 */
#define SCE_ERROR_ERRNO_EXDEV					-2147418094	/* 0x80010012 */

/**
 * @j デバイスが存在しない @ej
 * @e Associated device was not found @ee
 */
#define SCE_ERROR_ERRNO_ENODEV					-2147418093	/* 0x80010013 */

/**
 * @j ディレクトリではない @ej
 * @e Not a directory @ee
 */
#define SCE_ERROR_ERRNO_ENOTDIR					-2147418092	/* 0x80010014 */

/**
 * @j ディレクトリである @ej
 * @e Is a directory @ee
 */
#define SCE_ERROR_ERRNO_EISDIR					-2147418091	/* 0x80010015 */

/**
 * @j 引数が無効 @ej
 * @e Invalid argument @ee
 */
#define SCE_ERROR_ERRNO_EINVAL					-2147418090	/* 0x80010016 */

/**
 * @j オープン済みのファイルが多すぎる @ej
 * @e Too many files are open in the system @ee
 */
#define SCE_ERROR_ERRNO_ENFILE					-2147418089	/* 0x80010017 */

/**
 * @j オープン済みのファイルが多すぎる @ej
 * @e Too many files are open @ee
 */
#define SCE_ERROR_ERRNO_EMFILE					-2147418088	/* 0x80010018 */

/**
 * @j TTYデバイスではない @ej
 * @e Not a typewriter @ee
 */
#define SCE_ERROR_ERRNO_ENOTTY					-2147418087	/* 0x80010019 */

/**
 * @j テキストファイルが使用中である @ej
 * @e Text file is busy @ee
 */
#define SCE_ERROR_ERRNO_ETXTBSY					-2147418086	/* 0x8001001A */

/**
 * @j ファイルが巨大 @ej
 * @e File is too big @ee
 */
#define SCE_ERROR_ERRNO_EFBIG					-2147418085	/* 0x8001001B */

/**
 * @j デバイスにスペースが存在しない @ej
 * @e No free space on device @ee
 */
#define SCE_ERROR_ERRNO_ENOSPC					-2147418084	/* 0x8001001C */

/**
 * @j 無効なシーク @ej
 * @e Illegal seek @ee
 */
#define SCE_ERROR_ERRNO_ESPIPE					-2147418083	/* 0x8001001D */

/**
 * @j 読み込みのみのファイルシステム @ej
 * @e Read-only file system @ee
 */
#define SCE_ERROR_ERRNO_EROFS					-2147418082	/* 0x8001001E */

/**
 * @j リンクが多すぎる @ej
 * @e Too many links @ee
 */
#define SCE_ERROR_ERRNO_EMLINK					-2147418081	/* 0x8001001F */

/**
 * @j パイプが破壊された @ej
 * @e The pipe was broken @ee
 */
#define SCE_ERROR_ERRNO_EPIPE					-2147418080	/* 0x80010020 */

/**
 * @j 数値引数が領域外 @ej
 * @e Numeric argument is outside the area @ee
 */
#define SCE_ERROR_ERRNO_EDOM					-2147418079	/* 0x80010021 */

/**
 * @j 数値結果が範囲外 @ej
 * @e Numeric result is out of range @ee
 */
#define SCE_ERROR_ERRNO_ERANGE					-2147418078	/* 0x80010022 */

/**
 * @j 要求された型のメッセージがない。 @ej
 * @e A message of the requested type was not found @ee
 */
#define SCE_ERROR_ERRNO_ENOMSG					-2147418077	/* 0x80010023 */

/**
 * @j 識別子が削除された @ej
 * @e The identifier was deleted @ee
 */
#define SCE_ERROR_ERRNO_EIDRM					-2147418076	/* 0x80010024 */

/**
 * @j チャンネル番号が範囲外 @ej
 * @e Channel number is out of range @ee
 */
#define SCE_ERROR_ERRNO_ECHRNG					-2147418075	/* 0x80010025 */

/**
 * @j レベル2が非同期 @ej
 * @e Level 2 is asynchronous @ee
 */
#define SCE_ERROR_ERRNO_EL2NSYNC				-2147418074	/* 0x80010026 */

/**
 * @j レベル3が中断 @ej
 * @e Level 3 was halted @ee
 */
#define SCE_ERROR_ERRNO_EL3HLT					-2147418073	/* 0x80010027 */

/**
 * @j レベル3がリセット @ej
 * @e Level 3 was reset @ee
 */
#define SCE_ERROR_ERRNO_EL3RST					-2147418072	/* 0x80010028 */

/**
 * @j リンク番号が範囲外 @ej
 * @e Link number is out of range @ee
 */
#define SCE_ERROR_ERRNO_ELNRNG					-2147418071	/* 0x80010029 */

/**
 * @j プロトコルドライバが付いてない @ej
 * @e Protocol driver is no attached @ee
 */
#define SCE_ERROR_ERRNO_EUNATCH					-2147418070	/* 0x8001002A */

/**
 * @j CSI構造が利用できない @ej
 * @e CSI structure cannot be used @ee
 */
#define SCE_ERROR_ERRNO_ENOCSI					-2147418069	/* 0x8001002B */

/**
 * @j レベル2が中断 @ej
 * @e Level 2 was halted @ee
 */
#define SCE_ERROR_ERRNO_EL2HLT					-2147418068	/* 0x8001002C */

/**
 * @j デッドロック状態 @ej
 * @e Deadlock @ee
 */
#define SCE_ERROR_ERRNO_EDEADLK					-2147418067	/* 0x8001002D */

/**
 * @j ロックが利用できない @ej
 * @e Lock cannot be used @ee
 */
#define SCE_ERROR_ERRNO_ENOLCK					-2147418066	/* 0x8001002E */

/**
 * @j 不正なファイルフォーマット @ej
 * @e Invalid file format @ee
 */
#define SCE_ERROR_ERRNO_EFORMAT					-2147418065	/* 0x8001002F */

/**
 * @j デバイスがサポートしないオペレーション @ej
 * @e Operation is not supported by device @ee
 */
#define SCE_ERROR_ERRNO_EUNSUP					-2147418064	/* 0x80010030 */

/**
 * @j 無効なやり取り @ej
 * @e Invalid exchange @ee
 */
#define SCE_ERROR_ERRNO_EBADE					-2147418062	/* 0x80010032 */

/**
 * @j 無効な要求記述子 @ej
 * @e Invalid request identifier @ee
 */
#define SCE_ERROR_ERRNO_EBADR					-2147418061	/* 0x80010033 */

/**
 * @j やり取りがフル状態 @ej
 * @e Exchange is full @ee
 */
#define SCE_ERROR_ERRNO_EXFULL					-2147418060	/* 0x80010034 */

/**
 * @j anode がない @ej
 * @e anode was not found @ee
 */
#define SCE_ERROR_ERRNO_ENOANO					-2147418059	/* 0x80010035 */

/**
 * @j 無効な要求コード @ej
 * @e Invalid request code @ee
 */
#define SCE_ERROR_ERRNO_EBADRQC					-2147418058	/* 0x80010036 */

/**
 * @j 無効なスロット @ej
 * @e Invalid slot @ee
 */
#define SCE_ERROR_ERRNO_EBADSLT					-2147418057	/* 0x80010037 */

/**
 * @j ファイルロックデッドロックエラー @ej
 * @e File lock deadlock error @ee
 */
#define SCE_ERROR_ERRNO_EDEADLOCK				-2147418056	/* 0x80010038 */

/**
 * @j 不正なフォントのファイルフォーマット @ej
 * @e Invalid font file format @ee
 */
#define SCE_ERROR_ERRNO_EBFONT					-2147418055	/* 0x80010039 */

/**
 * @j ストリームしないデバイス @ej
 * @e Non-streaming device @ee
 */
#define SCE_ERROR_ERRNO_ENOSTR					-2147418052	/* 0x8001003C */

/**
 * @j データがない(遅延 io なし) @ej
 * @e No data was found (no delayed io) @ee
 */
#define SCE_ERROR_ERRNO_ENODATA					-2147418051	/* 0x8001003D */

/**
 * @j タイマ期限切れ @ej
 * @e Timer period expired @ee
 */
#define SCE_ERROR_ERRNO_ETIME					-2147418050	/* 0x8001003E */

/**
 * @j ストリームリソースの範囲を超えている @ej
 * @e Exceeded range of stream resource @ee
 */
#define SCE_ERROR_ERRNO_ENOSR					-2147418049	/* 0x8001003F */

/**
 * @j マシンがネットワーク上にない。 @ej
 * @e Machine is not on a network @ee
 */
#define SCE_ERROR_ERRNO_ENONET					-2147418048	/* 0x80010040 */

/**
 * @j パッケージがインストールされていない @ej
 * @e Package has not been installed @ee
 */
#define SCE_ERROR_ERRNO_ENOPKG					-2147418047	/* 0x80010041 */

/**
 * @j オブジェクトが遠隔操作される @ej
 * @e Object is remotely operated @ee
 */
#define SCE_ERROR_ERRNO_EREMOTE					-2147418046	/* 0x80010042 */

/**
 * @j リンクが切断される @ej
 * @e Link is disconnected @ee
 */
#define SCE_ERROR_ERRNO_ENOLINK					-2147418045	/* 0x80010043 */

/**
 * @j エラーの通知 @ej
 * @e Error notification @ee
 */
#define SCE_ERROR_ERRNO_EADV					-2147418044	/* 0x80010044 */

/**
 * @j srmount エラー @ej
 * @e srmount error @ee
 */
#define SCE_ERROR_ERRNO_ESRMNT					-2147418043	/* 0x80010045 */

/**
 * @j 送信時通信エラー @ej
 * @e Communication error while sending @ee
 */
#define SCE_ERROR_ERRNO_ECOMM					-2147418042	/* 0x80010046 */

/**
 * @j プロトコルエラー @ej
 * @e Protocol error @ee
 */
#define SCE_ERROR_ERRNO_EPROTO					-2147418041	/* 0x80010047 */

/**
 * @j multihop 試行済み @ej
 * @e multiop was attempted @ee
 */
#define SCE_ERROR_ERRNO_EMULTIHOP				-2147418038	/* 0x8001004A */

/**
 * @j node がリモート(エラーではない) @ej
 * @e node is remote (this is not a real error) @ee
 */
#define SCE_ERROR_ERRNO_ELBIN					-2147418037	/* 0x8001004B */

/**
 * @j クロスマウントポイント(エラーではない) @ej
 * @e cross mount point (this isn't a real error) @ee
 */
#define SCE_ERROR_ERRNO_EDOTDOT					-2147418036	/* 0x8001004C */

/**
 * @j 判読不能メッセージ読み込み試行中 @ej
 * @e Attempted to read undecipherable message @ee
 */
#define SCE_ERROR_ERRNO_EBADMSG					-2147418035	/* 0x8001004D */

/**
 * @j ファイルタイプエラー @ej
 */
#define SCE_ERROR_ERRNO_EFTYPE					-2147418033	/* 0x8001004F */

/**
 * @j ネットワーク上で名前が一意でない @ej
 * @e Name is not unique on the network @ee
 */
#define SCE_ERROR_ERRNO_ENOTUNIQ				-2147418032	/* 0x80010050 */

/**
 * @j ファイル記述子がこの処理で不正 @ej
 * @e File descriptor is invalid for this operation @ee
 */
#define SCE_ERROR_ERRNO_EBADFD					-2147418031	/* 0x80010051 */

/**
 * @j リモートアドレスが変更された @ej
 * @e Remote address was modified @ee
 */
#define SCE_ERROR_ERRNO_EREMCHG					-2147418030	/* 0x80010052 */

/**
 * @j 必要な共有ライブラリにアクセス不能 @ej
 * @e Cannot access required shared library @ee
 */
#define SCE_ERROR_ERRNO_ELIBACC					-2147418029	/* 0x80010053 */

/**
 * @j 破損した共有ライブラリにアクセス中 @ej
 * @e Accessed damaged shared library @ee
 */
#define SCE_ERROR_ERRNO_ELIBBAD					-2147418028	/* 0x80010054 */

/**
 * @j a.out の .lib セクションが破損 @ej
 * @e lib section of a.out is damaged @ee
 */
#define SCE_ERROR_ERRNO_ELIBSCN					-2147418027	/* 0x80010055 */

/**
 * @j 多くのライブラリでリンクを試行中 @ej
 * @e Attempted to link to too many libraries @ee
 */
#define SCE_ERROR_ERRNO_ELIBMAX					-2147418026	/* 0x80010056 */

/**
 * @j 共有ライブラリの実行を試行中 @ej
 * @e Attempted to execute shared library @ee
 */
#define SCE_ERROR_ERRNO_ELIBEXEC				-2147418025	/* 0x80010057 */

/**
 * @j 関数が実装されていない @ej
 * @e Function is not implemented @ee
 */
#define SCE_ERROR_ERRNO_ENOSYS					-2147418024	/* 0x80010058 */

/**
 * @j これ以上ファイルがない @ej
 * @e No more files were found @ee
 */
#define SCE_ERROR_ERRNO_ENMFILE					-2147418023	/* 0x80010059 */

/**
 * @j ディレクトリが空ではない @ej
 * @e Directory is not empty @ee
 */
#define SCE_ERROR_ERRNO_ENOTEMPTY				-2147418022	/* 0x8001005A */

/**
 * @j ファイル名またはパス名が長すぎる @ej
 * @e File name or path name is too long @ee
 */
#define SCE_ERROR_ERRNO_ENAMETOOLONG			-2147418021	/* 0x8001005B */

/**
 * @j シンボリックリンク追跡不可 @ej
 * @e Too many symbolic links encountered @ee
 */
#define SCE_ERROR_ERRNO_ELOOP					-2147418020	/* 0x8001005C */

/**
 * @j 操作がサポートされていません @ej
 * @e Operation is not supported @ee
 */
#define SCE_ERROR_ERRNO_EOPNOTSUPP				-2147418017	/* 0x8001005F */

/**
 * @j プロトコルファミリがサポートされていない @ej
 * @e Protocol family is not supported @ee
 */
#define SCE_ERROR_ERRNO_EPFNOSUPPORT			-2147418016	/* 0x80010060 */

/**
 * @j 接続がリセットされた @ej
 * @e Connection was reset by communications peer @ee
 */
#define SCE_ERROR_ERRNO_ECONNRESET				-2147418008	/* 0x80010068 */

/**
 * @j バッファがない @ej
 * @e Not enough free space in buffer @ee
 */
#define SCE_ERROR_ERRNO_ENOBUFS					-2147418007	/* 0x80010069 */

/**
 * @j アドレスがプロトコルによってサポートされてない @ej
 * @e Address family is not supported by protocol family @ee
 */
#define SCE_ERROR_ERRNO_EAFNOSUPPORT			-2147418006	/* 0x8001006A */

/**
 * @j ソケットのプロトコルタイプが不正 @ej
 * @e Socket protocol type is invalid @ee
 */
#define SCE_ERROR_ERRNO_EPROTOTYPE				-2147418005	/* 0x8001006B */

/**
 * @j ソケットでないものにソケット操作している @ej
 * @e Socket operation was performed for a non-socket object @ee
 */
#define SCE_ERROR_ERRNO_ENOTSOCK				-2147418004	/* 0x8001006C */

/**
 * @j プロトコルが使用できない @ej
 * @e Protocol cannot be used @ee
 */
#define SCE_ERROR_ERRNO_ENOPROTOOPT				-2147418003	/* 0x8001006D */

/**
 * @j ソケットの切断後で送信できない @ej
 * @e Cannot send after socket is shutdown @ee
 */
#define SCE_ERROR_ERRNO_ESHUTDOWN				-2147418002	/* 0x8001006E */

/**
 * @j 接続が拒否された @ej
 * @e Connection was refused @ee
 */
#define SCE_ERROR_ERRNO_ECONNREFUSED			-2147418001	/* 0x8001006F */

/**
 * @j アドレスが使用されている @ej
 * @e Address is already in use @ee
 */
#define SCE_ERROR_ERRNO_EADDRINUSE				-2147418000	/* 0x80010070 */

/**
 * @j 接続が中止された @ej
 * @e Connection was aborted by software @ee
 */
#define SCE_ERROR_ERRNO_ECONNABORTED			-2147417999	/* 0x80010071 */

/**
 * @j ネットワークに到達できない @ej
 * @e Network is unreachable @ee
 */
#define SCE_ERROR_ERRNO_ENETUNREACH				-2147417998	/* 0x80010072 */

/**
 * @j ネットワークがダウン @ej
 * @e Network is down @ee
 */
#define SCE_ERROR_ERRNO_ENETDOWN				-2147417997	/* 0x80010073 */

/**
 * @j タイムアウトした @ej
 * @e Operation timed out @ee
 */
#define SCE_ERROR_ERRNO_ETIMEDOUT				-2147417996	/* 0x80010074 */

/**
 * @j ホストがダウン @ej
 * @e Host is down @ee
 */
#define SCE_ERROR_ERRNO_EHOSTDOWN				-2147417995	/* 0x80010075 */

/**
 * @j ホストへの経路がない @ej
 * @e No route to the host was found @ee
 */
#define SCE_ERROR_ERRNO_EHOSTUNREACH			-2147417994	/* 0x80010076 */

/**
 * @j 操作が現在進行中 @ej
 * @e Operation is currently in progress @ee
 */
#define SCE_ERROR_ERRNO_EINPROGRESS				-2147417993	/* 0x80010077 */

/**
 * @j 操作は既に進行中 @ej
 * @e Operation is already in progress @ee
 */
#define SCE_ERROR_ERRNO_EALREADY				-2147417992	/* 0x80010078 */

/**
 * @j 送信先アドレスが要求されている @ej
 * @e Sending destination address has been requested @ee
 */
#define SCE_ERROR_ERRNO_EDESTADDRREQ			-2147417991	/* 0x80010079 */

/**
 * @j メッセージが長すぎる @ej
 * @e Message is too long @ee
 */
#define SCE_ERROR_ERRNO_EMSGSIZE				-2147417990	/* 0x8001007A */

/**
 * @j プロトコルがサポートされていない @ej
 * @e Protocol is not supported @ee
 */
#define SCE_ERROR_ERRNO_EPROTONOSUPPORT			-2147417989	/* 0x8001007B */

/**
 * @j ソケットタイプがサポートされていない @ej
 * @e Socket type is not supported @ee
 */
#define SCE_ERROR_ERRNO_ESOCKTNOSUPPORT			-2147417988	/* 0x8001007C */

/**
 * @j 要求されたアドレスを割り当てられない @ej
 * @e Requested address has not been allocated @ee
 */
#define SCE_ERROR_ERRNO_EADDRNOTAVAIL			-2147417987	/* 0x8001007D */

/**
 * @j リセットによりネットワークが切断された @ej
 * @e Network was shut down by a reset @ee
 */
#define SCE_ERROR_ERRNO_ENETRESET				-2147417986	/* 0x8001007E */

/**
 * @j ソケットは既に接続中 @ej
 * @e Socket is already connected @ee
 */
#define SCE_ERROR_ERRNO_EISCONN					-2147417985	/* 0x8001007F */

/**
 * @j ソケットは接続されていない @ej
 * @e Socket has not been connected @ee
 */
#define SCE_ERROR_ERRNO_ENOTCONN				-2147417984	/* 0x80010080 */

/**
 * @j リファレンスが多すぎる 接続不可能 @ej
 * @e Too many references, Cannot connect @ee
 */
#define SCE_ERROR_ERRNO_ETOOMANYREFS			-2147417983	/* 0x80010081 */

/**
 * @j プロセスが多すぎる @ej
 * @e Too many processes @ee
 */
#define SCE_ERROR_ERRNO_EPROCLIM				-2147417982	/* 0x80010082 */

/**
 * @j ユーザが多すぎる @ej
 * @e Too many users @ee
 */
#define SCE_ERROR_ERRNO_EUSERS					-2147417981	/* 0x80010083 */

/**
 * @j ディスククオータ超え @ej
 * @e Quota exceeded @ee
 */
#define SCE_ERROR_ERRNO_EDQUOT					-2147417980	/* 0x80010084 */

/**
 * @j Stale NFS file handle @ej
 * @e Stale NFS file handle @ee
 */
#define SCE_ERROR_ERRNO_ESTALE					-2147417979	/* 0x80010085 */

/**
 * @j サポートしていない @ej
 * @e Not supported @ee
 */
#define SCE_ERROR_ERRNO_ENOTSUP					-2147417978	/* 0x80010086 */

/**
 * @j メディアが見つからない @ej
 * @e No media was found @ee
 */
#define SCE_ERROR_ERRNO_ENOMEDIUM				-2147417977	/* 0x80010087 */

/**
 * @j 共有名が見つからない @ej
 * @e Shared name not found @ee
 */
#define SCE_ERROR_ERRNO_ENOSHARE				-2147417976	/* 0x80010088 */

/**
 * @j クラッシュ @ej
 */
#define SCE_ERROR_ERRNO_ECASECLASH				-2147417975	/* 0x80010089 */

/**
 * @j ILSEQ @ej
 */
#define SCE_ERROR_ERRNO_EILSEQ					-2147417974	/* 0x8001008A */

/**
 * @j オーバーフロー @ej
 * @e Overflow @ee
 */
#define SCE_ERROR_ERRNO_EOVERFLOW				-2147417973	/* 0x8001008B */

/**
 * @j キャンセルされた @ej
 */
#define SCE_ERROR_ERRNO_ECANCELED				-2147417972	/* 0x8001008C */

/**
 * @j 復帰可能ではない @ej
 */
#define SCE_ERROR_ERRNO_ENOTRECOVERABLE			-2147417971	/* 0x8001008D */

/**
 * @j 所有者が存在しない @ej
 */
#define SCE_ERROR_ERRNO_EOWNERDEAD				-2147417970	/* 0x8001008E */

/**
 * @j 改ざんが見つかった @ej
 * @e integrity check error @ee
 */
#define SCE_ERROR_ERRNO_EICV					-2147417969	/* 0x8001008F */



#endif	/* _VENEZIA_ERRNO_H */
