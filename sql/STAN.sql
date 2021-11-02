SELECT
/* Запрос по станциям */
c.UP_NOM    as KOD,
c.NOM       as PUT,
'STN'       as OBJ,
d.CODE      as NUMB,
e.NAME      as NAME,
k1.KM_MIN   as B_KM,
m1.M_MIN    as B_M,
d.KM        as O_KM,
d.m         as O_M,
k2.KM_MAX   as E_KM,
m2.M_MAX    as E_M,
0           as LEN


FROM WAY c , STANKM d, UP e, STR f,

(select MIN(KM) as KM_MIN, ST_KOD, UP_NOM, PUT_NOM
from  STR  group by ST_KOD,UP_NOM,PUT_NOM) k1,

(select MIN(M) as M_MIN, KM, ST_KOD, UP_NOM, PUT_NOM
from  STR  group by ST_KOD,UP_NOM,PUT_NOM, KM) m1,

(select MAX(KM) as KM_MAX, ST_KOD, UP_NOM, PUT_NOM
from  STR  group by ST_KOD,UP_NOM,PUT_NOM) k2,

(select MAX(M) as M_MAX, KM, ST_KOD, UP_NOM, PUT_NOM
from  STR  group by ST_KOD,UP_NOM,PUT_NOM, KM) m2



/* WHERE c.assetnum = '110000123030' */
WHERE c.up_nom = :DIR_CODE and c.nom = :TRACK_NUM
/* Параметр  c.assetnum = :P из папспорта проезда Atape */
/* and c.assetnum = :P */
and d.up_nom   = c.up_nom
and e.up_nom   = d.code
and f.up_nom  =  c.up_nom
and f.put_nom  = c.nom
and f.st_kod   = d.code
and k1.st_kod  = d.code
and k1.up_nom  = c.up_nom
and k1.put_nom = c.nom
and m1.st_kod  = d.code
and m1.up_nom  = c.up_nom
and m1.put_nom = c.nom
and k1.KM_MIN  = m1.KM
and k2.st_kod  = d.code
and k2.up_nom  = c.up_nom
and k2.put_nom = c.nom
and m2.st_kod  = d.code
and m2.up_nom  = c.up_nom
and m2.put_nom = c.nom
and k2.KM_MAX  = m2.KM


group by
KOD,
PUT,
OBJ,
B_KM,
B_M,
O_KM,
O_M,
E_KM,
E_M,
NUMB,
NAME,
LEN