SELECT
/* Запрос по участкам дистанций пути */
b.UP_NOM    as KOD,
b.PUT_NOM   as PUT,
'PCH'       as OBJ,
b.NUM       as NUMB,
d.type      as type, 
b.BEGIN_KM  as B_KM,
b.BEGIN_M   as B_M,
0           as O_KM,
0           as O_M,
b.END_KM    as E_KM,
b.END_M     as E_M,
0           as LEN

FROM PODR a, PODRGR b, WAY c, spr_podr d


/* WHERE c.assetnum = '110000123030' */
WHERE c.up_nom = :DIR_CODE and c.nom = :TRACK_NUM
/* Параметр  c.assetnum = :P из папспорта проезда Atape */
/* and c.assetnum = :P */
/* and c.assetnum = '110000122959' */
and c.up_nom   = b.up_nom
and c.nom      = b.put_nom
and a.num      = b.num
and a.type     = b.type
and a.type     = d.id
and a.type  = 5

ORDER by
B_KM,
B_M