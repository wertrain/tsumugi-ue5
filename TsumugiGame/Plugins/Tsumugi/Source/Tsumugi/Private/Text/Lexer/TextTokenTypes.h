#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::lexer {

/// <summary>
/// 繝医・繧ｯ繝ｳ遞ｮ鬘・
/// </summary>
enum class TokenType : int32_t {

    /// <summary>
    /// 荳肴ｭ｣縺ｪ繝医・繧ｯ繝ｳ
    /// </summary>
    kIllegal,

    /// <summary>
    /// 邨らｫｯ繝弱・繝・(End of File)
    /// </summary>
    kEOF,

    /// <summary>
    /// 險伜捷莉･螟悶・逕溘・譁・ｭ怜・繝ｻ隴伜挨蟄・
    /// ・亥慍縺ｮ譁・√Λ繝吶Ν蜷阪∬ｦ句・縺怜錐縲√ち繧ｰ蜷阪∝ｱ樊ｧ蜷阪∝ｱ樊ｧ蛟､縺ｮ縺吶∋縺ｦ縺後％繧後↓縺ｪ繧翫∪縺呻ｼ・
    /// </summary>
    kString,

    /// <summary>
    /// 謾ｹ陦・
    /// </summary>
    kNewLine,

    /// <summary>
    /// 陦碁ｭ縺ｮ繝ｩ繝吶Ν螳夂ｾｩ繝槭・繧ｯ (*)
    /// </summary>
    kAsterisk,

    /// <summary>
    /// 陦碁ｭ縺ｮ繧ｳ繝槭Φ繝牙ｮ夂ｾｩ繝槭・繧ｯ (@)
    /// </summary>
    kAtMark,

    /// <summary>
    /// 繝ｩ繝吶Ν縺ｮ隕句・縺励そ繝代Ξ繝ｼ繧ｿ繝ｼ (|)
    /// </summary>
    kPipe,

    /// <summary>
    /// 繧ｿ繧ｰ縺ｮ髢句ｧ九き繝・さ ([)
    /// </summary>
    kTagOpen,

    /// <summary>
    /// 繧ｿ繧ｰ縺ｮ髢峨§繧ｫ繝・さ (])
    /// </summary>
    kTagClose,

    /// <summary>
    /// 繧ｿ繧ｰ螻樊ｧ縺ｮ莉｣蜈･譁・ｭ・(=)
    /// </summary>
    kAssign,

    /// <summary>
    /// 陦碁ｭ縺ｮ繧ｳ繝｡繝ｳ繝医・繝ｼ繧ｯ (;)
    /// </summary>
    kSemiColon
};

/// <summary>
/// 繝医・繧ｯ繝ｳ繧ｿ繧､繝励ｒ譁・ｭ怜・縺ｫ螟画峩縺吶ｋ
/// </summary>
/// <param name="token_type"></param>
/// <returns></returns>
const tchar* TokenTypeToTString(const TokenType token_type);
const char* TokenTypeToString(const TokenType token_type);

}
